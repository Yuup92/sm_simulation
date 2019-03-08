//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

message BasicMessage
{
    int source;
    int destination;
    int scalar_clock;
    int src_node_id;
    
    bool ack;
        
    int root_node;
    bool start_spanning_tree = false;
    
    bool spanning_request = false;
    
    bool spanning_request_ack = false;
    int spanning_tree_level;
    
    bool spanning_decline_request = false;
    
    
}