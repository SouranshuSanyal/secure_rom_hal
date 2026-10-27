
`timescale 1ns/1ps

module secure_periph_tb;

    reg clk;
    reg reset_n;
    reg [3:0] addr;
    reg write_en;
    reg read_en;
    reg [31:0] write_data;
    reg [3:0] write_strobe;

    wire [31:0] read_data;
    wire [31:0] gpio_out;

    secure_peripheral dut (
        .clk(clk),
        .reset_n(reset_n),
        .addr(addr),
        .write_en(write_en),
        .read_en(read_en),
        .write_data(write_data),
        .write_strobe(write_strobe),
        .read_data(read_data),
        .gpio_out(gpio_out)
    );

    // 100 MHz clock: 10 ns period
    initial clk = 1'b0;
    always #5 clk = ~clk;

    // Write one register

    task write_register;
        input [3:0] register_addr;
        input [31:0] data;
        input [3:0] strobe;
        begin
            @(negedge clk);
            addr = register_addr;
            write_data = data;
            write_strobe = strobe;
            write_en = 1'b1;

            @(negedge clk);
            write_en = 1'b0;
            write_data = 32'h0;
            write_strobe = 4'b0000;
        end
    endtask


    // Check one register's read value
    task check_register;
        input [3:0] register_addr;
        input [31:0] expected;
        begin
            @(negedge clk);
            addr = register_addr;
            read_en = 1'b1;
            #1;

            if (read_data !== expected) begin
                $display(
                    "FAIL addr=%h expected=%h actual=%h",
                    register_addr, expected, read_data
                );
                $fatal(1);
            end
            else begin
                $display(
                    "PASS addr=%h data=%h",
                    register_addr, read_data
                );
            end

            read_en = 1'b0;
        end
    endtask

    initial begin
        reset_n = 1'b0;
        addr = 4'h0;
        write_en = 1'b0;
        read_en = 1'b0;
        write_data = 32'h0;

        // Keep reset active across clock edges
        repeat (2) @(negedge clk);
        reset_n = 1'b1;

        // Check reset values
        check_register(4'h0, 32'h00000000);
        check_register(4'h8, 32'h00000000);

        // Enable peripheral
        write_register(4'h0, 32'h00000001, 4'b1111);
        check_register(4'h0, 32'h00000001);
        check_register(4'h4, 32'h00000001);

        // Write GPIO output
        write_register(4'h8, 32'h000000A5, 4'b1111);
        check_register(4'h8, 32'h000000A5);

        // Initialize output register to 0x11223344
        write_register(4'h8, 32'h11223344, 4'b1111);
        check_register(4'h8, 32'h11223344);

        // Update only byte 0
        write_register(4'h8, 32'h000000AA, 4'b0001);
        check_register(4'h8, 32'h112233AA);

        // Update only byte 2
        write_register(4'h8, 32'h00BB0000, 4'b0100);
        check_register(4'h8, 32'h11BB33AA);

        // Zero strobe must not change any byte
        write_register(4'h8, 32'hFFFFFFFF, 4'b0000);
        check_register(4'h8, 32'h11BB33AA);

        if (gpio_out !== 8'hAA) begin
            $display(
                "FAIL gpio_out expected=A5 actual=%h",
                gpio_out
            );
            $fatal(1);
        end
        else begin
            $display("PASS gpio_out = %h", gpio_out);
        end

        // Invalid address returns zero
        check_register(4'hC, 32'h00000000);

        $display("ALL RTL TESTS PASSED");
        $finish;
    end

    initial begin
        #1000;
        $display("FAIL: simulation timeout");
        $fatal(1);
    end

endmodule
