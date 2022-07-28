export class Debug {
  constructor(
    public numPacketsSent = 0,
    public numPacketsReceived = 0,
    public RTT = 0,
    public packetLoss = 0,
    public sentBandwidth = 0,
    public receivedBandwidth = 0,
  ) {
  }
}
