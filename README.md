MPTCP and RFC 9438 Simulation in NS-3 for D2D Communication
🔧 1. NS-3 Installation
        📄 Please refer to the Installing ns-3.txt file for complete instructions on setting up the environment, installing dependencies, and building NS-3 (v3.38).

📁 2. Simulation Setup
        Place your simulation file (e.g., adhoc-2nodes.cc) inside the scratch/ directory.
        
        Ensure your mobility trace file (e.g., 10nodes.dat.ns_movements) is in the appropriate path.

  To run the simulation:

          ./ns3 --run "scratch/adhoc-2nodes"
          

📡 3. Simulation Parameters

        | Parameter           | Value                         |
        |---------------------|-------------------------------|
        | TCP Protocol        | MPTCP, RFC 9438               |
        | WiFi Standard       | 802.11n                       |
        | Number of Clusters  | 10                            |
        | Nodes per Cluster   | 20                            |
        | MaxBytes            | Unlimited (0)                 |
        | Simulation Time     | 1 to 100 seconds              |
        | Node Speed          | 0.5 m/s, 1 m/s, 1.5 m/s, 2 m/s |
        | Output Visualization| NetAnim, FlowMonitor, PyVis   |



📈 4. Key Metrics Evaluated
          1)Throughput (Mbps)
          2)Latency (ms)
          3)Efficiency (% useful data)
          4)Packet Loss

🎞 5. Visualization Tools
NetAnim: XML file (adhoc-2nodes.xml) for mobility + packet flow.

FlowMonitor: adhoc-2nodes.flowmon for performance stats.

PyVis: Visual graph of node interactions (cluster-level views).
