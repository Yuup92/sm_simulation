//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc11
{
    parameters:
        @display("i=block/routing");
    gates:
        input in[];  // declare in[] and out[] to be vector gates
        output out[];
}


//
// Using local channel type definition to reduce the redundancy
// of connection definitions.
//
network Tictoc11
{
    @display("bgb=273,269;bgl=6");
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 100ms;
        }
    submodules:
        tic: Txc11 {
            @display("p=143,64");
        }
        
        toc: Txc11;
        tac: Txc11;
        tec: Txc11;
        
        tex: Txc11;
        tax: Txc11;
    connections:
        tic.out++ --> Channel --> toc.in++;
        tic.in++ <-- Channel <-- toc.out++;

        toc.out++ --> Channel --> tac.in++;
        toc.in++ <-- Channel <-- tac.out++;

        toc.out++ --> Channel --> tex.in++;
        toc.in++ <-- Channel <-- tex.out++;

        tic[3].out++ --> Channel --> tic[4].in++;
        tic[3].in++ <-- Channel <-- tic[4].out++;

        tic[4].out++ --> Channel --> tic[5].in++;
        tic[4].in++ <-- Channel <-- tic[5].out++;
}
