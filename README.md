# Fully Linear PCP Simulator for Inner Product Circuits

This repository is the C++ implementation of the improved version of fully linear PCP for inner product circuit.
[Fully linear PCP (FLPCP)](https://eprint.iacr.org/2019/188.pdf) is zero-knowledge proof scheme which can be efficiently applied on distributed or secret-shared data by leveraging short linear proofs.

This project has been developed on Visual Studio CMake project and Windows 11 environment.

## Project Structure

* `circuit` - logic constructing proofs and queries of FLPCP and FLIOP.
* `experiments` - performance measurement of primitive modular operations, and simulation logic of 2PC, 3PC using FLPCP and FLIOP.
* `figure` - experiment result graphs and its poltting Python code.
* `math` - logic of primitive modular operations and polynomial interpolation.
* `unit` - class representing proof and query.
* `win-x64-release` - complied executable files for x64 windows environment.

## Experiments

### Environment

* CPU: AMD Ryzen 9 7950X 16-Core Processor @ 5.00 GHz (Only used a single core)
* Memory: DDR5 32GB x2

### FLPCP Complexity

<div align="center">
  <table>
    <tr>
      <th> Prover Time </th>
      <th> Verifier Time </th>
      <th> Proof size </th>
    </tr>
    <tr>
      <td> <img src="./figure/FLPCP/prover_time.png" /> </td>
      <td> <img src="./figure/FLPCP/verifier_time.png" /> </td>
      <td> <img src="./figure/FLPCP/proof_size.png" /> </td>
    </tr>
  </table>
</div>

### FLIOP Complexity

<div align="center">
  <table>
    <tr>
      <th> Prover Time </th>
      <th> Verifier Time </th>
      <th> Proof Size </th>
    </tr>
    <tr>
      <td> <img src="./figure/FLIOP/prover_time.png" /> </td>
      <td> <img src="./figure/FLIOP/verifier_time.png" /> </td>
      <td> <img src="./figure/FLIOP/proof_size.png" /> </td>
    </tr>
  </table>
</div>

### Finding optimal schedules in FLIOP

* Total Improvement

<div align="center">
  <table>
    <tr>
      <th> Total Improvement in LAN </th>
      <th> Proportion in LAN </th>
    </tr>
    <tr>
      <td> <img src="./figure/optimal_schedule/lan_total.png" /> </td>
      <td> <img src="./figure/optimal_schedule/lan_portion.png" /> </td>
    </tr>
    <tr>
      <th> Total Improvement in WAN </th>
      <th> Proportion in WAN </th>
    </tr>
    <tr>
      <td> <img src="./figure/optimal_schedule/wan_total.png" /> </td>
      <td> <img src="./figure/optimal_schedule/wan_portion.png" /> </td>
    </tr>
  </table>
</div>

* Optimal schedule in LAN

```
Length: 32 / Min time : 0.043880690 / Best schedule : 6 3 2
Length: 64 / Min time : 0.049097384 / Best schedule : 5 7 2
Length: 128 / Min time : 0.060402949 / Best schedule : 5 4 4 2
Length: 256 / Min time : 0.080319643 / Best schedule : 6 5 5 2
Length: 512 / Min time : 0.121041902 / Best schedule : 6 6 8 2
Length: 1024 / Min time : 0.198241902 / Best schedule : 6 5 5 4 2
Length: 2048 / Min time : 0.353958597 / Best schedule : 6 7 5 5 2
Length: 4096 / Min time : 0.663364162 / Best schedule : 6 6 5 4 3 2
Length: 8192 / Min time : 1.282280856 / Best schedule : 6 6 6 5 4 2
Length: 16384 / Min time : 2.532403115 / Best schedule : 5 6 6 7 7 2
Length: 32768 / Min time : 5.052508680 / Best schedule : 6 6 6 6 4 4 2
Length: 65536 / Min time : 10.526725374 / Best schedule : 7 6 6 6 5 5 2
```

* Optimal schedule in WAN

```
Length: 32 / Min time : 40.037000000 / Best schedule : 6 3 2
Length: 64 / Min time : 40.044900000 / Best schedule : 4 4 2 2
Length: 128 / Min time : 40.059900000 / Best schedule : 5 4 4 2
Length: 256 / Min time : 40.084600000 / Best schedule : 6 5 5 2
Length: 512 / Min time : 40.127500000 / Best schedule : 6 5 3 3 2
Length: 1024 / Min time : 40.208900000 / Best schedule : 6 5 5 4 2
Length: 2048 / Min time : 40.369000000 / Best schedule : 6 5 3 4 3 2
Length: 4096 / Min time : 40.680400000 / Best schedule : 6 6 5 4 3 2
Length: 8192 / Min time : 41.303800000 / Best schedule : 6 6 5 4 3 2 2
Length: 16384 / Min time : 42.556300000 / Best schedule : 5 6 6 4 4 3 2
Length: 32768 / Min time : 45.081000000 / Best schedule : 6 6 6 5 4 4 2
Length: 65536 / Min time : 50.561000000 / Best schedule : 7 6 6 5 3 3 3 2
```

### Comparison between baseline and coefficient version in MPC

<div align="center">
  <table>
    <tr>
      <th> LAN </th>
      <th> WAN </th>
    </tr>
    <tr>
      <td> <img src="./figure/Network%20Simulation/LAN_precomputed_oracle.png" /> </td>
      <td> <img src="./figure/Network%20Simulation/WAN_precomputed_oracle.png" /> </td>
    </tr>
  </table>
</div>

## Reference

* Zero-Knowledge Proofs on Secret-Shared Data via Fully Linear PCPs (2019) [[PDF](https://eprint.iacr.org/2019/188.pdf)]
