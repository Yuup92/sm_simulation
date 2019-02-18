simple BasicNode
{
    parameters:
        @display("i=misc/node");
    gates:
        input in[];
        output out[];
}

network CompleteNetwork7
{
    @display("bgb=436,425");
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100ms;
        }

    submodules:
        node_0: BasicNode {
            @display("p=164,38");
        }
        node_1: BasicNode {

            @display("p=52,87");
        }
        node_2: BasicNode {
            @display("p=103,250");
        }
        node_3: BasicNode {
            @display("p=35,188");
        }
        node_4: BasicNode {
            @display("p=219,259");
        }
        node_5: BasicNode {
            @display("p=281,202");
        }
        node_6: BasicNode {
            @display("p=303,87");
        }
    connections:
        //Node0
        node_0.out++ --> Channel --> node_1.in++;
        node_0.in++ <-- Channel <-- node_1.out++;

        node_0.out++ --> Channel --> node_2.in++;
        node_0.in++ <-- Channel <-- node_2.out++;

        node_0.out++ --> Channel --> node_3.in++;
        node_0.in++ <-- Channel <-- node_3.out++;

        node_0.out++ --> Channel --> node_4.in++;
        node_0.in++ <-- Channel <-- node_4.out++;

        node_0.out++ --> Channel --> node_5.in++;
        node_0.in++ <-- Channel <-- node_5.out++;

        node_0.out++ --> Channel --> node_6.in++;
        node_0.in++ <-- Channel <-- node_6.out++;
        
        
        // Node1
        node_1.out++ --> Channel --> node_2.in++;
        node_1.in++ <-- Channel <-- node_2.out++;

        node_1.out++ --> Channel --> node_3.in++;
        node_1.in++ <-- Channel <-- node_3.out++;

        node_1.out++ --> Channel --> node_4.in++;
        node_1.in++ <-- Channel <-- node_4.out++;

        node_1.out++ --> Channel --> node_5.in++;
        node_1.in++ <-- Channel <-- node_5.out++;

        node_1.out++ --> Channel --> node_6.in++;
        node_1.in++ <-- Channel <-- node_6.out++;      
        
        //Node2
        node_2.out++ --> Channel --> node_3.in++;
        node_2.in++ <-- Channel <-- node_3.out++;

        node_2.out++ --> Channel --> node_4.in++;
        node_2.in++ <-- Channel <-- node_4.out++;

        node_2.out++ --> Channel --> node_5.in++;
        node_2.in++ <-- Channel <-- node_5.out++;

        node_2.out++ --> Channel --> node_6.in++;
        node_2.in++ <-- Channel <-- node_6.out++;
        
        // Node3
        node_3.out++ --> Channel --> node_4.in++;
        node_3.in++ <-- Channel <-- node_4.out++;

        node_3.out++ --> Channel --> node_5.in++;
        node_3.in++ <-- Channel <-- node_5.out++;

        node_3.out++ --> Channel --> node_6.in++;
        node_3.in++ <-- Channel <-- node_6.out++;
        
        // Node4
        node_4.out++ --> Channel --> node_5.in++;
        node_4.in++ <-- Channel <-- node_5.out++;

        node_4.out++ --> Channel --> node_6.in++;
        node_4.in++ <-- Channel <-- node_6.out++;
        
        // Node5
        node_5.out++ --> Channel --> node_6.in++;
        node_5.in++ <-- Channel <-- node_6.out++;      
}



