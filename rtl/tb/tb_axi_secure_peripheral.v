
`timescale 1ns/1ps

module tb_axi_secure_peripheral;

    reg clk = 1'b0;
    reg reset_n = 1'b0;

    reg [3:0] awaddr = 0;
    reg awvalid = 0;
    wire awready;

    reg [31:0] wdata = 0;
    reg [3:0] wstrb = 0;
    reg wvalid = 0;
    wire wready;

    wire [1:0] bresp;
    wire bvalid;
    reg bready = 1;

    reg [3:0] araddr = 0;
    reg arvalid = 0;
    wire arready;

    wire [31:0] rdata;
    wire [1:0] rresp;
    wire rvalid;
    reg rready = 1;

    wire [7:0] gpio_out;

    integer errors = 0;

    localparam [1:0] RESP_OKAY   = 2'b00;
    localparam [1:0] RESP_DECERR = 2'b11;

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

    // Write address first, then write data.
    task axi_write;
        input [3:0] address;
        input [31:0] data;
        input [3:0] strobe;
        input address_first;
        begin
            if (address_first) begin
                @(negedge clk);
                awaddr = address;
                awvalid = 1'b1;

                do @(posedge clk);
                while (awready !== 1'b1);

                @(negedge clk);
                awvalid = 1'b0;

                repeat (2) @(negedge clk);

                wdata = data;
                wstrb = strobe;
                wvalid = 1'b1;

                do @(posedge clk);
                while (wready !== 1'b1);

                @(negedge clk);
                wvalid = 1'b0;
                wstrb = 4'b0000;
            end
            else begin
                @(negedge clk);
                wdata = data;
                wstrb = strobe;
                wvalid = 1'b1;

                do @(posedge clk);
                while (wready !== 1'b1);

                @(negedge clk);
                wvalid = 1'b0;
                wstrb = 4'b0000;

                repeat (2) @(negedge clk);

                awaddr = address;
                awvalid = 1'b1;

                do @(posedge clk);
                while (awready !== 1'b1);

                @(negedge clk);
                awvalid = 1'b0;
            end

            wait (bvalid === 1'b1);

            if (bresp === RESP_OKAY)
                $display("PASS write addr=%h data=%h strb=%b",
                         address, data, strobe);
            else if (bresp === RESP_DECERR)
                $display("PASS write rejected addr=%h BRESP=DECERR",
                         address);
            else begin
                $display("FAIL unexpected BRESP=%b", bresp);
                errors = errors + 1;
            end

            @(posedge clk);
            @(negedge clk);
        end
    endtask

    task axi_read;
        input [3:0] address;
        input [31:0] expected_data;
        input [1:0] expected_response;
        begin
            @(negedge clk);
            araddr = address;
            arvalid = 1'b1;

            do @(posedge clk);
            while (arready !== 1'b1);

            @(negedge clk);
            arvalid = 1'b0;

            wait (rvalid === 1'b1);

            if ((rdata !== expected_data) ||
                (rresp !== expected_response)) begin
                $display(
                    "FAIL read addr=%h expected=%h actual=%h resp=%b",
                    address, expected_data, rdata, rresp
                );
                errors = errors + 1;
            end
            else begin
                $display(
                    "PASS read addr=%h data=%h resp=%b",
                    address, rdata, rresp
                );
            end

            @(posedge clk);
            @(negedge clk);
        end
    endtask

    initial begin
        // Reset
        repeat (3) @(negedge clk);
        reset_n = 1'b1;

        axi_read(4'h0, 32'h00000000, RESP_OKAY);
        axi_read(4'h8, 32'h00000000, RESP_OKAY);

        // Address arrives before data.
        axi_write(4'h0, 32'h00000001, 4'b1111, 1'b1);
        axi_read(4'h0, 32'h00000001, RESP_OKAY);
        axi_read(4'h4, 32'h00000001, RESP_OKAY);

        // Data arrives before address.
        axi_write(4'h8, 32'h11223344, 4'b1111, 1'b0);
        axi_read(4'h8, 32'h11223344, RESP_OKAY);

        // Partial writes.
        axi_write(4'h8, 32'h000000AA, 4'b0001, 1'b1);
        axi_read(4'h8, 32'h112233AA, RESP_OKAY);

        axi_write(4'h8, 32'h00BB0000, 4'b0100, 1'b0);
        axi_read(4'h8, 32'h11BB33AA, RESP_OKAY);

        // No byte lanes selected: register must not change.
        axi_write(4'h8, 32'hFFFFFFFF, 4'b0000, 1'b1);
        axi_read(4'h8, 32'h11BB33AA, RESP_OKAY);

        // GPIO reflects the low output-register byte.
        if (gpio_out !== 8'hAA) begin
            $display("FAIL GPIO expected=AA actual=%h", gpio_out);
            errors = errors + 1;
        end
        else begin
            $display("PASS GPIO output=%h", gpio_out);
        end

        // STATUS is read-only.
        axi_write(4'h4, 32'hFFFFFFFF, 4'b1111, 1'b1);
        axi_read(4'h4, 32'h00000001, RESP_OKAY);

        // Unmapped read and write.
        axi_read(4'hC, 32'h00000000, RESP_DECERR);
        axi_write(4'hC, 32'h12345678, 4'b1111, 1'b0);

        if (errors == 0)
            $display("ALL AXI TESTS PASSED");
        else
            $display("AXI TESTS FAILED: %0d error(s)", errors);

        $finish;
    end

    initial begin
        #10000;
        $display("FAIL: simulation timeout");
        $fatal(1);
    end

endmodule
