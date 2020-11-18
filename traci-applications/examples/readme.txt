ns3-sumo-coupling-simple.cc  --> 自带的圆环场景仿真
line-test ---> 和traci/example下的line-simple场景相对应，在line-control.cc文件里进行了场景判断，更改速度等
echo-test ---> 测试如何和通信模型相结合的echo场景
brake-test --> 测试前车刹车、后车通过socket接收到消息后减速并刹车的场景