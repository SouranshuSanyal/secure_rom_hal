
module axi_secure_peripheral (
    input  wire        s_axi_aclk,
    input  wire        s_axi_aresetn,

    // Write address channel
    input  wire [3:0]  s_axi_awaddr,
    input  wire        s_axi_awvalid,
    output wire        s_axi_awready,

    // Write data channel
    input  wire [31:0] s_axi_wdata,
    input  wire [3:0]  s_axi_wstrb,
    input  wire        s_axi_wvalid,
    output wire        s_axi_wready,

    // Write response channel
    output reg  [1:0]  s_axi_bresp,
    output reg         s_axi_bvalid,
    input  wire        s_axi_bready,

    // Read address channel
    input  wire [3:0]  s_axi_araddr,
    input  wire        s_axi_arvalid,
    output wire        s_axi_arready,

    // Read data/response channel
    output reg  [31:0] s_axi_rdata,
    output reg  [1:0]  s_axi_rresp,
    output reg         s_axi_rvalid,
    input  wire        s_axi_rready,

    // Peripheral output
    output wire [7:0]  gpio_out
);

    localparam [1:0] RESP_OKAY   = 2'b00;
    localparam [1:0] RESP_DECERR = 2'b11;

    localparam [3:0] ADDR_CONTROL = 4'h0;
    localparam [3:0] ADDR_STATUS  = 4'h4;
    localparam [3:0] ADDR_OUTPUT  = 4'h8;

    reg [31:0] control_reg;
    reg [31:0] output_reg;

    // Hold independently received write address and data.
    reg        aw_pending;
    reg        w_pending;
    reg [3:0]  awaddr_saved;
    reg [31:0] wdata_saved;
    reg [3:0]  wstrb_saved;

    wire aw_handshake = s_axi_awvalid && s_axi_awready;
    wire w_handshake  = s_axi_wvalid  && s_axi_wready;

    // One outstanding write response at a time.
    assign s_axi_awready = !aw_pending && !s_axi_bvalid;
    assign s_axi_wready  = !w_pending  && !s_axi_bvalid;

    // One outstanding read response at a time.
    assign s_axi_arready = !s_axi_rvalid;

    // Register write channel handling.
    always @(posedge s_axi_aclk or negedge s_axi_aresetn) begin
        if (!s_axi_aresetn) begin
            control_reg <= 32'h00000000;
            output_reg  <= 32'h00000000;

            aw_pending  <= 1'b0;
            w_pending   <= 1'b0;
            awaddr_saved <= 4'h0;
            wdata_saved  <= 32'h0;
            wstrb_saved  <= 4'h0;

            s_axi_bresp  <= RESP_OKAY;
            s_axi_bvalid <= 1'b0;
        end
        else begin
            // Accept address independently.
            if (aw_handshake) begin
                awaddr_saved <= s_axi_awaddr;
                aw_pending   <= 1'b1;
            end

            // Accept data independently.
            if (w_handshake) begin
                wdata_saved <= s_axi_wdata;
                wstrb_saved <= s_axi_wstrb;
                w_pending   <= 1'b1;
            end

            // Complete a write once both pieces are available.
            if (aw_pending && w_pending && !s_axi_bvalid) begin
                case (awaddr_saved)
                    ADDR_CONTROL: begin
                        if (wstrb_saved[0])
                            control_reg[7:0] <= wdata_saved[7:0];

                        if (wstrb_saved[1])
                            control_reg[15:8] <= wdata_saved[15:8];

                        if (wstrb_saved[2])
                            control_reg[23:16] <= wdata_saved[23:16];

                        if (wstrb_saved[3])
                            control_reg[31:24] <= wdata_saved[31:24];

                        s_axi_bresp <= RESP_OKAY;
                    end

                    ADDR_OUTPUT: begin
                        if (wstrb_saved[0])
                            output_reg[7:0] <= wdata_saved[7:0];

                        if (wstrb_saved[1])
                            output_reg[15:8] <= wdata_saved[15:8];

                        if (wstrb_saved[2])
                            output_reg[23:16] <= wdata_saved[23:16];

                        if (wstrb_saved[3])
                            output_reg[31:24] <= wdata_saved[31:24];

                        s_axi_bresp <= RESP_OKAY;
                    end

                    // STATUS is read-only. Other addresses are unmapped.
                    default: begin
                        s_axi_bresp <= RESP_DECERR;
                    end
                endcase

                aw_pending  <= 1'b0;
                w_pending   <= 1'b0;
                s_axi_bvalid <= 1'b1;
            end

            // Hold BVALID and BRESP until the master accepts the response.
            if (s_axi_bvalid && s_axi_bready)
                s_axi_bvalid <= 1'b0;
        end
    end

    // Read channel: capture the selected register on a read handshake.
    always @(posedge s_axi_aclk or negedge s_axi_aresetn) begin
        if (!s_axi_aresetn) begin
            s_axi_rdata  <= 32'h00000000;
            s_axi_rresp  <= RESP_OKAY;
            s_axi_rvalid <= 1'b0;
        end
        else begin
            if (s_axi_arvalid && s_axi_arready) begin
                case (s_axi_araddr)
                    ADDR_CONTROL: begin
                        s_axi_rdata <= control_reg;
                        s_axi_rresp <= RESP_OKAY;
                    end

                    ADDR_STATUS: begin
                        s_axi_rdata <= {31'b0, control_reg[0]};
                        s_axi_rresp <= RESP_OKAY;
                    end

                    ADDR_OUTPUT: begin
                        s_axi_rdata <= output_reg;
                        s_axi_rresp <= RESP_OKAY;
                    end

                    default: begin
                        s_axi_rdata <= 32'h00000000;
                        s_axi_rresp <= RESP_DECERR;
                    end
                endcase

                s_axi_rvalid <= 1'b1;
            end

            // Hold RVALID, RDATA and RRESP until accepted.
            if (s_axi_rvalid && s_axi_rready)
                s_axi_rvalid <= 1'b0;
        end
    end

    assign gpio_out = output_reg[7:0];

endmodule
