module secure_peripheral (
    input wire clk,
    input wire reset_n,
    input wire [3:0] addr,
    input wire write_en,
    input wire read_en,
    input wire [3:0] write_strobe,
    input wire [31:0] write_data,
    output reg  [31:0] read_data,
    output reg [31:0] gpio_out
);

localparam ADDR_CONTROL = 4'h0;
localparam ADDR_STATUS = 4'h4;
localparam ADDR_OUTPUT = 4'h8;

reg [31:0] control_reg;
reg [31:0] output_reg;


always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        control_reg <= 32'h00000000;
        output_reg  <= 32'h00000000;
    end
    else if (write_en) begin
        case (addr)

            ADDR_CONTROL: begin
                if (write_strobe[0])
                    control_reg[7:0] <= write_data[7:0];

                if (write_strobe[1])
                    control_reg[15:8] <= write_data[15:8];

                if (write_strobe[2])
                    control_reg[23:16] <= write_data[23:16];

                if (write_strobe[3])
                    control_reg[31:24] <= write_data[31:24];
            end

            ADDR_OUTPUT: begin
                if (write_strobe[0])
                    output_reg[7:0] <= write_data[7:0];

                if (write_strobe[1])
                    output_reg[15:8] <= write_data[15:8];

                if (write_strobe[2])
                    output_reg[23:16] <= write_data[23:16];

                if (write_strobe[3])
                    output_reg[31:24] <= write_data[31:24];
            end

            default: begin
                // Ignore writes to unmapped addresses
            end

        endcase
    end
end



always @(*) begin
    read_data = 0;
    if (read_en) begin
            case(addr)
                ADDR_CONTROL: read_data = control_reg;
                ADDR_STATUS: read_data = {31'b0,control_reg[0]};
                ADDR_OUTPUT: read_data = output_reg;

                default: read_data = 0;
            endcase
    end
end

assign gpio_out = output_reg[7:0];

endmodule