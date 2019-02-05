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
        node0: BasicNode {
            @display("p=164,38");
        }
        node1: BasicNode {

            @display("p=52,87");
        }
        node2: BasicNode {
            @display("p=103,250");
        }
        node3: BasicNode {
            @display("p=35,188");
        }
        node4: BasicNode {
            @display("p=219,259");
        }
        node5: BasicNode {
            @display("p=281,202");
        }
        node6: BasicNode {
            @display("p=303,87");
        }
    connections:
        //Node0
        node0.out++ --> Channel --> node1.in++;
        node0.in++ <-- Channel <-- node1.out++;

        node0.out++ --> Channel --> node2.in++;
        node0.in++ <-- Channel <-- node2.out++;

        node0.out++ --> Channel --> node3.in++;
        node0.in++ <-- Channel <-- node3.out++;

        node0.out++ --> Channel --> node4.in++;
        node0.in++ <-- Channel <-- node4.out++;

        node0.out++ --> Channel --> node5.in++;
        node0.in++ <-- Channel <-- node5.out++;

        node0.out++ --> Channel --> node6.in++;
        node0.in++ <-- Channel <-- node6.out++;
        
        
        // Node1
        node1.out++ --> Channel --> node2.in++;
        node1.in++ <-- Channel <-- node2.out++;

        node1.out++ --> Channel --> node3.in++;
        node1.in++ <-- Channel <-- node3.out++;

        node1.out++ --> Channel --> node4.in++;
        node1.in++ <-- Channel <-- node4.out++;

        node1.out++ --> Channel --> node5.in++;
        node1.in++ <-- Channel <-- node5.out++;

        node1.out++ --> Channel --> node6.in++;
        node1.in++ <-- Channel <-- node6.out++;      
        
        //Node2
        node2.out++ --> Channel --> node3.in++;
        node2.in++ <-- Channel <-- node3.out++;

        node2.out++ --> Channel --> node4.in++;
        node2.in++ <-- Channel <-- node4.out++;

        node2.out++ --> Channel --> node5.in++;
        node2.in++ <-- Channel <-- node5.out++;

        node2.out++ --> Channel --> node6.in++;
        node2.in++ <-- Channel <-- node6.out++;
        
        // Node3
        node3.out++ --> Channel --> node4.in++;
        node3.in++ <-- Channel <-- node4.out++;

        node3.out++ --> Channel --> node5.in++;
        node3.in++ <-- Channel <-- node5.out++;

        node3.out++ --> Channel --> node6.in++;
        node3.in++ <-- Channel <-- node6.out++;
        
        // Node4
        node4.out++ --> Channel --> node5.in++;
        node4.in++ <-- Channel <-- node5.out++;

        node4.out++ --> Channel --> node6.in++;
        node4.in++ <-- Channel <-- node6.out++;
        
        // Node5
        node5.out++ --> Channel --> node6.in++;
        node5.in++ <-- Channel <-- node6.out++;      
}



