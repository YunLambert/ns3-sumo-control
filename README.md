# NS3-SUMO-CONTROL

Thanks to vodafone-chair's work:[ns3-smo-coupling](https://github.com/vodafone-chair/ns3-sumo-coupling), it's easy for us to focus on the logic of service.

## How To Build?

The test environment is SUMO(>=1.0.0) and NS3.26 . SUMO's Traci changed from the version 1.0.0, so it is recommended to use the after version.

Move the "traci" and "traci-applications" two directories into the ns3/src and build like new module.

Config files of sumo are in traci/examples/XX-simple.

## Version

2020-11-5 

Demo for test. Add the line-simple and related code in the traci-applications. 

In line-simple, we can :

- from sumo to ns3: get speed, position and ids...
- from ns3 to sumo: change speed, change max speed (for test, so I only write this two interfaces....)

There is a little video I record for the test.

2020-11-14:

Make the interface in LineControl public as API for other module to use.

Add tutorial/first.cc which I have changed it to use sumo-ns3-control, just to show how to use sumo-control-control in Mobility and Communication environment.

- [ ] Remove deprecated function and value.
- [ ] Add Doxygen-like doc
- [ ] Add more api
- [ ] FIX: the simulation in NS3 is too fast, the data from the traci may be inaccurate. For example, I change speed to 5m/s at time 11.0000s, but the ns3 may give me 1.0067m/s at time 11.0006s(the sumulation time)