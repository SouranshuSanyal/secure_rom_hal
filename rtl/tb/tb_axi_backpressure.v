
`timescale 1ns/1ps

module tb_axi_backpressure;

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
    integer i;

    reg [31:0] saved_rdata;
    reg [1:0] saved_rresp;
    reg [1:0] saved_bresp;

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

    // Write one register and deliberately stall the
    // write response by keeping BREADY low.
    task test_write_backpressure;
        begin
            $display("\nTEST 1: Write response backpressure");

            // Send write address.
            @(negedge clk);
            awaddr = 4'h8;
            awvalid = 1'b1;

            do begin
                @(posedge clk);
            end while (awready !== 1'b1);

            @(negedge clk);
            awvalid = 1'b0;

            // Send write data.
            wdata = 32'h12345678;
            wstrb = 4'b1111;
            wvalid = 1'b1;

            do begin
                @(posedge clk);
            end while (wready !== 1'b1);

            @(negedge clk);
            wvalid = 1'b0;

            // Wait for the write response.
            wait (bvalid === 1'b1);
            #1;
            saved_bresp = bresp;

            if (bresp !== 2'b00) begin
                $display("FAIL: Expected OKAY write response");
                errors = errors + 1;
            end

            // Master refuses the response for 3 clocks.
            for (i = 0; i < 3; i = i + 1) begin
                @(posedge clk);
                #1;

                if (bvalid !== 1'b1) begin
                    $display("FAIL: BVALID dropped while BREADY=0");
                    errors = errors + 1;
                end

                if (bresp !== saved_bresp) begin
                    $display("FAIL: BRESP changed under backpressure");
                    errors = errors + 1;
                end
            end

            $display("BVALID and BRESP held while BREADY=0");

            // Accept the response.
            @(negedge clk);
            bready = 1'b1;

            @(posedge clk);
            #1;

            if (bvalid !== 1'b0) begin
                $display("FAIL: BVALID did not clear after handshake");
                errors = errors + 1;
            end else begin
                $display("PASS: Write response completed");
            end

            @(negedge clk);
            bready = 1'b0;
        end
    endtask

    // Read the OUTPUT register and deliberately stall
    // the read response by keeping RREADY low.
    task test_read_backpressure;
        begin
            $display("\nTEST 2: Read response backpressure");

            // Issue read address for OUTPUT (0x8).
            @(negedge clk);
            araddr = 4'h8;
            arvalid = 1'b1;

            do begin
                @(posedge clk);
            end while (arready !== 1'b1);

            @(negedge clk);
            arvalid = 1'b0;

            // Wait for the read response.
            wait (rvalid === 1'b1);
            #1;

            saved_rdata = rdata;
            saved_rresp = rresp;

            if (saved_rdata !== 32'h12345678) begin
                $display("FAIL: Incorrect read data: %h", saved_rdata);
                errors = errors + 1;
            end

            if (saved_rresp !== 2'b00) begin
                $display("FAIL: Expected OKAY read response");
                errors = errors + 1;
            end

            // Master refuses the response for 3 clocks.
            for (i = 0; i < 3; i = i + 1) begin
                @(posedge clk);
                #1;

                if (rvalid !== 1'b1) begin
                    $display("FAIL: RVALID dropped while RREADY=0");
                    errors = errors + 1;
                end

                if (rdata !== saved_rdata) begin
                    $display("FAIL: RDATA changed under backpressure");
                    errors = errors + 1;
                end

                if (rresp !== saved_rresp) begin
                    $display("FAIL: RRESP changed under backpressure");
                    errors = errors + 1;
                end
            end

            $display("RVALID, RDATA and RRESP held while RREADY=0");

            // Accept the read response.
            @(negedge clk);
            rready = 1'b1;

            @(posedge clk);
            #1;

            if (rvalid !== 1'b0) begin
                $display("FAIL: RVALID did not clear after handshake");
                errors = errors + 1;
            end else begin
                $display("PASS: Read response completed");
            end

            @(negedge clk);
            rready = 1'b0;
        end
    endtask

    initial begin
        clk = 1'b0;
        reset_n = 1'b0;

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
        saved_rdata = 0;
        saved_rresp = 0;
        saved_bresp = 0;

        repeat (4) @(posedge clk);
        @(negedge clk);
        reset_n = 1'b1;

        repeat (2) @(posedge clk);

        test_write_backpressure();
        test_read_backpressure();

        $display("\n--------------------------------");
        if (errors == 0)
            $display("ALL BACKPRESSURE TESTS PASSED");
        else
            $display("BACKPRESSURE TESTS FAILED: %0d errors", errors);
        $display("--------------------------------");

        $finish;
    end

    initial begin
        #10000;
        $display("ERROR: Simulation timeout");
        $finish;
    end

endmodule
