# AHB arbiter version 2.0

We have re written the AHB arbiter specification to be able to scale it properly using our enumerative tools.
The original specification has been splitted into five components, two of which are simple mux/demux instances.

The Masters' **Mux** and **Demux** are omitted since their behavior is sufficiently derived from their name.

Remember that there is also a **Decoder** selecting each slave according to it's corresponding address range.

## SCHEDULER
The way in which grants are give is decided by the `SCHEDULER` module.

*Signals*

1. **Input:**
    * [HBusReq1 .. HBusReqN] coming from each master requiring access to the bus
    * [Decide] coming from the burst manager, indicating that a req should be picked
2. **Output:**
	* [Select] this is the `log2(N)` width input to the mux/demux instances

*Liveness Guarantees*

1. **(G11) forall i []<>(HBusReqi -> Select == i)** all requests should be evt. selected

*Liveness Assumptions*

1. **(G13) []<> Decide** the burst manager should keep asking to pick a new grant

## GRANT LATCH
The `GRANT LATCH` is responsible for keeping the `hgranti` and `hmaster` signals consistent with the original specification according to the input signals.

*Signals*

1. **Input:**
    * [HLock] coming from the masters' mux
    * [HGrant] coming from the burst manager
    * [HMastLock] coming from the burst manager
2. **Output:**
	* [Enable] enabling the masters' demux
	* [HMaster] an `N` width signal indicating which masters are currently granted

## BURST MANAGER
The `BURST MANAGER` is responsible for keeping the `hGrant`, `hMastLock` and `decide` signals consistent with input signals.

*Signals*

1. **Input:**
    * [HBusReq] coming from mux's output
    * [HLock] coming form mux's output
    * [HTrans(1:0)] transmission type
        * [00] `nonseq`
        * [01] `seq`
        * [10] `idle` 
    * [HBurst(1:0)] burst type
        * [00] `single`
        * [01] `incr`
        * [10] `incr4`
    * [HReady] coming form active slave
2. **Output:**
	* [HMastLock] indicates whether the current master has an exclusive lock on the bus
	* [HGrant] indicates whether the scheduled master is granted access to the bus
	* [Decide] indicates that the arbiter is on it's deciding phase
	* [Granted, BusReq] auxiliar signals

*Liveness Guarantees*

1. **(G2)** if in `incr` mode, `hTrans` should keep as `seq` until no `busreq`
2. **(G3)** if in `incr4` mode, `hTrans`should keep as `seq` during the next 4 `hready` occurrences
3. **(G10)** do not grant until requested

*Liveness Assumptions*

1. **(A1)** evt. lower grant after unbounded request