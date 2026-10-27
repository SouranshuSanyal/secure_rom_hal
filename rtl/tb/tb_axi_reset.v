
`timescale 1ns/1ps

module tb_axi_reset;

    reg clk;
    reg reset_n;

    reg [3:0] awaddr;
    reg awvalid;
    wire awready;

    reg [31:0] wdata;
    reg [3:0] wstrb;
    reg wvalid;
    wire wready;

    wire [1:0] bresp;
    wire bvalid;
    reg bready;

    reg [3:0] araddr;
    reg arvalid;
    wire arready;

    wire [31:0] rdata;
    wire [1:0] rresp;
    wire rvalid;
    reg rready;

    wire [7:0] gpio_out;

    integer errors;

    always #5 clk = ~clk;

    axi_secure_peripheral dut (
        .s_axi_aclk(clk),
        .s_axi_aresetn(reset_n),

        .s_axi_awaddr(awaddr),
        .s_axi_awvalid(awvalid),
        .s_axi_awready(awready),

        .s_axi_wdata(wdata),
        .s_axi_wstrb(wstrb),
        .s_axi_wvalid(wvalid),
        .s_axi_wready(wready),

        .s_axi_bresp(bresp),
        .s_axi_bvalid(bvalid),
        .s_axi_bready(bready),

        .s_axi_araddr(araddr),
        .s_axi_arvalid(arvalid),
        .s_axi_arready(arready),

        .s_axi_rdata(rdata),
        .s_axi_rresp(rresp),
        .s_axi_rvalid(rvalid),
        .s_axi_rready(rready),

        .gpio_out(gpio_out)
    );

    // AW and W are asserted together to test
    // simultaneous write-channel activity.
    // stall_response=1 leaves BREADY low.
    task issue_write;
        input [3:0] address;
        input [31:0] data;
        input stall_response;

        begin
            @(negedge clk);
            awaddr = address;
            wdata = data;
            wstrb = 4'b1111;
            awvalid = 1'b1;
            wvalid = 1'b1;

            // Both channels are presented together.
            do begin
                @(posedge clk);
            end while (!(awready && wready));

            @(negedge clk);
            awvalid = 1'b0;
            wvalid = 1'b0;

            wait (bvalid === 1'b1);
            #1;

            if (bresp !== 2'b00) begin
                $display("FAIL: Write response was not OKAY");
                errors = errors + 1;
            end

            if (!stall_response) begin
                @(negedge clk);
                bready = 1'b1;

                @(posedge clk);
                #1;

                @(negedge clk);
                bready = 1'b0;
            end
        end
    endtask

    task issue_read_stalled;
        input [3:0] address;

        begin
            @(negedge clk);
            araddr = address;
            arvalid = 1'b1;

            do begin
                @(posedge clk);
            end while (arready !== 1'b1);

            @(negedge clk);
            arvalid = 1'b0;

            wait (rvalid === 1'b1);
            #1;
        end
    endtask

    task release_read_response;
        begin
            @(negedge clk);
            rready = 1'b1;

            @(posedge clk);
            #1;

            @(negedge clk);
            rready = 1'b0;
        end
    endtask

    task apply_reset;
        begin
            // Assert reset away from the active clock edge.
            @(negedge clk);
            reset_n = 1'b0;

            // Allow reset to propagate through the RTL.
            repeat (2) @(posedge clk);
            #1;

            if (bvalid !== 1'b0) begin
                $display("FAIL: BVALID not cleared by reset");
                errors = errors + 1;
            end

            if (rvalid !== 1'b0) begin
                $display("FAIL: RVALID not cleared by reset");
                errors = errors + 1;
            end

            if (gpio_out !== 8'h00) begin
                $display("FAIL: GPIO not reset, value=%h", gpio_out);
                errors = errors + 1;
            end

            @(negedge clk);
            reset_n = 1'b1;

            repeat (2) @(posedge clk);
            #1;
        end
    endtask

    initial begin
        clk = 0;
        reset_n = 0;

        awaddr = 0;
        awvalid = 0;
        wdata = 0;
        wstrb = 0;
        wvalid = 0;
        bready = 0;

        araddr = 0;
        arvalid = 0;
        rready = 0;

        errors = 0;

        $display("Starting Day 45 AXI reset tests");

        // Initial reset and idle-state check.
        repeat (3) @(posedge clk);
        @(negedge clk);
        reset_n = 1'b1;

        repeat (2) @(posedge clk);
        #1;

        if (bvalid !== 1'b0 || rvalid !== 1'b0) begin
            $display("FAIL: Response valid during idle");
            errors = errors + 1;
        end else begin
            $display("PASS: Reset and idle state");
        end

        // Test 1: Reset while a write response is pending.
        $display("TEST: Reset with pending write response");

        issue_write(4'h8, 32'h000000A5, 1'b1);

        if (bvalid !== 1'b1) begin
            $display("FAIL: Expected pending BVALID");
            errors = errors + 1;
        end

        apply_reset();

        if (bvalid === 1'b0)
            $display("PASS: Pending write response cleared");

        // Test 2: Reset while a read response is pending.
        $display("TEST: Reset with pending read response");

        // First write a nonzero GPIO value and complete
        // its write response.
        issue_write(4'h8, 32'h0000005A, 1'b0);

        if (gpio_out !== 8'h5A) begin
            $display("FAIL: GPIO write before read test");
            errors = errors + 1;
        end

        issue_read_stalled(4'h8);

        if (rdata !== 32'h0000005A) begin
            $display("FAIL: Incorrect pending read data: %h", rdata);
            errors = errors + 1;
        end

        if (rvalid !== 1'b1) begin
            $display("FAIL: Expected pending RVALID");
            errors = errors + 1;
        end

        apply_reset();

        if (rvalid === 1'b0)
            $display("PASS: Pending read response cleared");

        // Test 3: Confirm register state after reset.
        $display("TEST: Register reset values");

        issue_read_stalled(4'h8);

        if (rdata !== 32'h00000000) begin
            $display("FAIL: OUTPUT register not reset: %h", rdata);
            errors = errors + 1;
        end else begin
            $display("PASS: OUTPUT register reset to zero");
        end

        release_read_response();

        issue_read_stalled(4'h0);

        if (rdata !== 32'h00000000) begin
            $display("FAIL: CONTROL register not reset: %h", rdata);
            errors = errors + 1;
        end else begin
            $display("PASS: CONTROL register reset to zero");
        end

        release_read_response();

        // Test 4: Normal write/read recovery after reset.
        $display("TEST: Transaction recovery after reset");

        issue_write(4'h8, 32'h000000C3, 1'b0);

        issue_read_stalled(4'h8);

        if (rdata !== 32'h000000C3) begin
            $display("FAIL: Post-reset transaction returned %h", rdata);
            errors = errors + 1;
        end else begin
            $display("PASS: Post-reset read/write recovery");
        end

        release_read_response();

        $display("----------------------------------");

        if (errors == 0)
            $display("ALL DAY 45 RESET TESTS PASSED");
        else
            $display("DAY 45 FAILED: %0d errors", errors);

        $display("----------------------------------");
        $finish;
    end

    initial begin
        #10000;
        $display("ERROR: Simulation timeout");
        $finish;
    end

endmodule
