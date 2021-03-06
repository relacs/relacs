
static nidaq_info nidaq_boards[KNOWN_BOARDS] = {
  { name: "PCI-MIO-16E-1",
    deviceid: 0x1180,
    type: NI_PCI_MIO_16E_1,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 1250000,
    aififo: 512,
    aiuniranges: { -1, 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1 },
    aibiranges: { 10000, 5000, 2500, 1000, 500, 250, 100, 50, -1, -1 },
    aiindices: { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 2048,
    dios: 8,
  },
  { name: "PXI-6070E",
    deviceid: 0x11b0,
    type: NI_PXI_6070E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 1250000,
    aififo: 512,
    aiuniranges: { -1, 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1 },
    aibiranges: { 10000, 5000, 2500, 1000, 500, 250, 100, 50, -1, -1 },
    aiindices: { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 2048,
    dios: 8,
  },
  { name: "PCI-6071E",
    deviceid: 0x1350,
    type: NI_PCI_6071E,
    aich: 64,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 1250000,
    aififo: 512,
    aiuniranges: { -1, 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1 },
    aibiranges: { 10000, 5000, 2500, 1000, 500, 250, 100, 50, -1, -1 },
    aiindices: { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 2048,
    dios: 8,
  },
  { name: "PXI-6071E",
    deviceid: 0x15b0,
    type: NI_PXI_6071E,
    aich: 64,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 1250000,
    aififo: 512,
    aiuniranges: { -1, 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1 },
    aibiranges: { 10000, 5000, 2500, 1000, 500, 250, 100, 50, -1, -1 },
    aiindices: { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 2048,
    dios: 8,
  },
  { name: "PCI-MIO-16E-4",
    deviceid: 0x1190,
    type: NI_PCI_MIO_16E_4,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 250000,
    aififo: 512,
    aiuniranges: { -1, 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1 },
    aibiranges: { 10000, 5000, 2500, 1000, 500, 250, 100, 50, -1, -1 },
    aiindices: { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PXI-6040E",
    deviceid: 0x11c0,
    type: NI_PXI_6040E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 250000,
    aififo: 512,
    aiuniranges: { -1, 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1 },
    aibiranges: { 10000, 5000, 2500, 1000, 500, 250, 100, 50, -1, -1 },
    aiindices: { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PCI-MIO-16XE-10",
    deviceid: 0x1170,
    type: NI_PCI_MIO_16XE_10,
    aich: 16,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 100000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1, -1 },
    aibiranges: { 5000, 2500, 1000, 500, 250, 100, 50, -1, -1, -1 },
    aiindices: { 1, 2, 3, 4, 5, 6, 7, -1, -1, -1 },
    aoch: 2,
    aobits: 16,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PXI-6030E",
    deviceid: 0x11d0,
    type: NI_PXI_6030E,
    aich: 16,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 100000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1, -1 },
    aibiranges: { 5000, 2500, 1000, 500, 250, 100, 50, -1, -1, -1 },
    aiindices: { 1, 2, 3, 4, 5, 6, 7, -1, -1, -1 },
    aoch: 2,
    aobits: 16,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PCI-6031E",
    deviceid: 0x1330,
    type: NI_PCI_6031E,
    aich: 64,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 100000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1, -1 },
    aibiranges: { 5000, 2500, 1000, 500, 250, 100, 50, -1, -1, -1 },
    aiindices: { 1, 2, 3, 4, 5, 6, 7, -1, -1, -1 },
    aoch: 2,
    aobits: 16,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PXI-6031E",
    deviceid: 0x1580,
    type: NI_PXI_6031E,
    aich: 64,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 100000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1, -1 },
    aibiranges: { 5000, 2500, 1000, 500, 250, 100, 50, -1, -1, -1 },
    aiindices: { 1, 2, 3, 4, 5, 6, 7, -1, -1, -1 },
    aoch: 2,
    aobits: 16,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PCI-6032E",
    deviceid: 0x1270,
    type: NI_PCI_6032E,
    aich: 16,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 100000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1, -1 },
    aibiranges: { 5000, 2500, 1000, 500, 250, 100, 50, -1, -1, -1 },
    aiindices: { 1, 2, 3, 4, 5, 6, 7, -1, -1, -1 },
    aoch: 0,
    aobits: 0,
    aomaxspl: 0,
    aofifo: 0,
    dios: 8,
  },
  { name: "PCI-6033E",
    deviceid: 0x1340,
    type: NI_PCI_6033E,
    aich: 64,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 100000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1, -1 },
    aibiranges: { 5000, 2500, 1000, 500, 250, 100, 50, -1, -1, -1 },
    aiindices: { 1, 2, 3, 4, 5, 6, 7, -1, -1, -1 },
    aoch: 0,
    aobits: 0,
    aomaxspl: 0,
    aofifo: 0,
    dios: 8,
  },
  { name: "PCI-6023E",
    deviceid: 0x2a60,
    type: NI_PCI_6023E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 200000,
    aififo: 512,
    aiuniranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 10000, 5000, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 0, 1, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 0,
    aobits: 0,
    aomaxspl: 0,
    aofifo: 0,
    dios: 8,
  },
  { name: "PCI-6024E",
    deviceid: 0x2a70,
    type: NI_PCI_6024E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 200000,
    aififo: 512,
    aiuniranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 10000, 5000, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 0, 1, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000,
    aofifo: 0,
    dios: 8,
  },
  { name: "PCI-6025E",
    deviceid: 0x2a80,
    type: NI_PCI_6025E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 200000,
    aififo: 512,
    aiuniranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 10000, 5000, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 0, 1, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 0,
    dios: 32,
  },
  { name: "PXI-6023E",
    deviceid: 0x2a90,
    type: NI_PXI_6023E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 200000,
    aififo: 512,
    aiuniranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 10000, 5000, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 0, 1, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 0,
    aobits: 0,
    aomaxspl: 0,
    aofifo: 0,
    dios: 8,
  },
  { name: "PXI-6024E",
    deviceid: 0x2aa0,
    type: NI_PXI_6024E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 200000,
    aififo: 512,
    aiuniranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 10000, 5000, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 0, 1, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000,
    aofifo: 0,
    dios: 8,
  },
  { name: "PXI-6025E",
    deviceid: 0x2ab0,
    type: NI_PXI_6025E,
    aich: 16,
    aimaxch: 256,
    aibits: 12,
    aimaxspl: 200000,
    aififo: 512,
    aiuniranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 10000, 5000, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 0, 1, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 0,
    dios: 32,
  },
  { name: "PCI-MIO-16XE-50",
    deviceid: 0x1620,
    type: NI_PCI_MIO_16XE_50,
    aich: 16,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 20000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 1000, 100, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 5000, 2500, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 1, 2, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PXI-6011E",
    deviceid: 0x11e0,
    type: NI_PXI_6011E,
    aich: 16,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 20000,
    aififo: 512,
    aiuniranges: { 10000, 5000, 1000, 100, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 5000, 2500, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 1, 2, 4, 7, -1, -1, -1, -1, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 512,
    dios: 8,
  },
  { name: "PCI-6052E",
    deviceid: 0x18b0,
    type: NI_PCI_6052E,
    aich: 16,
    aimaxch: 256,
    aibits: 16,
    aimaxspl: 333000,
    aififo: 512,
    aiuniranges: { -1, 10000, 5000, 2000, 1000, 500, 200, 100, -1, -1 },
    aibiranges: { 10000, 5000, 2500, 1000, 500, 250, 100, 50, -1, -1 },
    aiindices: { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1 },
    aoch: 2,
    aobits: 16,
    aomaxspl: 333000,
    aofifo: 2048,
    dios: 8,
  },
  { name: "PCI-6711",
    deviceid: 0x1880,
    type: NI_PCI_6711,
    aich: 0,
    aimaxch: 0,
    aibits: 0,
    aimaxspl: 0,
    aififo: 0,
    aiuniranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aibiranges: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aiindices: { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    aoch: 4,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 8192,
    dios: 8,
  },
  { name: "PCI-6035E",
    deviceid: 0x2c80,
    type: NI_PCI_6035E,
    aich: 16,
    aimaxch: 512,
    aibits: 16,
    aimaxspl: 200000,
    aififo: 512,
    aiuniranges: { 20000, 10000, 1000, 100, -1, -1, -1, -1, -1, -1 },
    aibiranges: { 10000, 5000, 500, 50, -1, -1, -1, -1, -1, -1 },
    aiindices: { 0, 1, 2, 3, -1, -1, -1, -1, -1, -1 },
    aoch: 2,
    aobits: 12,
    aomaxspl: 1000000,
    aofifo: 0,
    dios: 8,
  }
};
