#
# Copyright (C) 2017-2025 Tactical Computing Laboratories, LLC
# All Rights Reserved
# contact@tactcomplabs.com
#
# See LICENSE in the top level directory for licensing details
#
# rev-xbgas-test.py
#

# ---------------------------------------------------------------
#
#  xbgas_host0           xbgas_host1
#      |                     |
#  rmt_mem_ctrl0        rmt_mem_ctrl1    
#      |                     |  
#     nic0                  nic1
#      |                     |
#    iface0 <-> router <-> iface1
#                  |
#               topology
#
#   <-> is a link
#   | is a subcomponent relationship

import argparse
import os
import sst
import sys

parser = argparse.ArgumentParser(description="Run Rev SST Simulation")
parser.add_argument("--program", help="The program executable to run in the simulation", default="a.out")
parser.add_argument("--args", help="Command line arguments to pass to the target executable", default="")

# Parse arguments
args = parser.parse_args()

# Print arguments nicely
print("Rev SST Simulation Configuration:")
for arg in vars(args):
    print("\t", arg, " = ", getattr(args, arg))

NPES = 2
CLOCK = "2.5GHz"  
MEMSIZE = 1024*1024*1024
SHARED_MEM_SIZE = 1024*1024*16

memctrl_params = {
  "clock": CLOCK,
  "addr_range_start": 0,
  "addr_range_end": MEMSIZE-1,
  "backing": "malloc"
}

l1cache_params = {
  "access_latency_cycles" : "4",
  "cache_frequency" : CLOCK,
  "replacement_policy" : "lru",
  "coherence_protocol" : "MESI",
  "associativity" : "4",
  "cache_line_size" : "64",
  "L1" : "1",
  "cache_size" : "16KiB"
}

l2cache_params = {
  "access_latency_cycles" : "8",
  "mshr_latency_cycles": "2",
  "cache_frequency" : CLOCK,
  "replacement_policy" : "lru",
  "coherence_protocol" : "MESI",
  "associativity" : "8",
  "cache_line_size" : "64",
  "L1" : "0",  # Not an L1
  "cache_size" : "128KiB"
}

mem_params = {
  "access_time" : "100ns",
  "mem_size" : "8GB"
}

net_params = {
  "input_buf_size" : "512B",
  "output_buf_size" : "512B",
  "link_bw" : "25GB/s"
}

# setup the router
router = sst.Component("router", "merlin.hr_router")
router.setSubComponent("topology", "merlin.singlerouter")
router.addParams(net_params)

router.addParams({
    "xbar_bw" : "25GB/s",
    "flit_size" : "32B",
    "num_ports" : str(NPES),
    "id" : 0
})

for i in range(0, NPES):
  if i == 0:
    VERBOSE = 1
  else:
    VERBOSE = 1
  # xBGAS CPUs
  xbgas_cpu = sst.Component("cpu" + str(i), "revcpu.RevCPU")
  xbgas_cpu.addParams({
    "verbose" : VERBOSE,                          # Verbosity
    "clock" : CLOCK,                              # Clock
    "program" : args.program,                     # Target executable
    "memSize" : MEMSIZE,                          # Memory size in bytes
    "startAddr" : "[0:0x00000000]",               # Starting address for core 0
    "machine" : "[0:RV64GC_Xbgas]",
    "memCost" : "[0:1:10]",                       # Memory loads required 1-10 cycles
    "enable_xbgas" : 1,                           # Enable XBGAS support 
    "enableMemH": 1,                              # Enable memHierarchy support
    "args": args.args,                            # Command line arguments
    "shared_memory_size": SHARED_MEM_SIZE,        # Shared memory size
    "splash" : 0                                  # Display the splash message
  })
  # print("Created xBGAS CPU component " + str(i) + ": " + xbgas_cpu.getFullName())
  
  # xbgas_cpu.enableAllStatistics()
  
  # Setup the memory controllers
  lsq = xbgas_cpu.setSubComponent("memory", "revcpu.RevBasicMemCtrl")

  # Create the memHierarchy subcomponent
  miface = lsq.setSubComponent("memIface", "memHierarchy.standardInterface")

  # Create the L1 cache
  l1cache = sst.Component("l1cache" + str(i), "memHierarchy.Cache")
  l1cache.addParams(l1cache_params)

  # Create the L2 cache
  l2cache = sst.Component("l2cache" + str(i), "memHierarchy.Cache")
  l2cache.addParams(l2cache_params)
  
  # Create the memory controller in memHierarchy
  memctrl = sst.Component("memory" + str(i), "memHierarchy.MemController")
  memctrl.addParams(memctrl_params)

  # Create the memory backend subcomponent
  memory = memctrl.setSubComponent("backend", "memHierarchy.simpleMem")
  memory.addParams(mem_params)
  
  # setup the links

  # Connect CPU to L1
  link_miface_l1cache = sst.Link("link_miface_l1cache" + str(i))
  link_miface_l1cache.connect((miface, "port", "1ns"), (l1cache, "high_network_0", "1ns"))

  # Connect L1 to L2
  link_l1cache_l2 = sst.Link("link_l1cache_l2" + str(i))
  link_l1cache_l2.connect((l1cache, "low_network_0", "10ns"), (l2cache, "high_network_0", "10ns"))

  # Connect L2 to memory controller
  link_l2_mem = sst.Link("link_l2_mem" + str(i))
  link_l2_mem.connect((l2cache, "low_network_0", "40ns"), (memctrl, "direct_link", "40ns"))

  # Create remote memory controllers
  rmt_lsq = xbgas_cpu.setSubComponent("remote_memory", "revcpu.RevBasicRmtMemCtrl")
  rmt_nic = rmt_lsq.setSubComponent("xbgasNicIface", "revcpu.XbgasNIC")
  rmt_nic_iface = rmt_nic.setSubComponent("iface", "merlin.linkcontrol")

  rmt_nic_iface.addParams(net_params)
  
  link = sst.Link("link" + str(i))
  link.connect( (rmt_nic_iface, "rtr_port", "100ns"), (router, f"port{i}", "100ns") )


# Tell SST what statistics handling we want
# sst.setStatisticLoadLevel(2)
# sst.setStatisticOutput("sst.statOutputCSV")

# EOF
