/*
 *@Programmer: Tyler Raborn
 *@Description: This class implements an undirected weighted graph via an adjacency list, which is implemented as a wrapper class for an array of linked lists.
 *@Development Environment: Eclipse Kepler, Windows 8 Pro 64-bit, JDK7.0_21
 *@Due: 7/29/2013
 */

import java.io.*;
import java.util.*;
import java.lang.*;

public class AdjacencyList implements UndirectedGraph 
{

	private int edgeCount;
	private int vertexCount; //holds initial number of vertices. This determines the size of the wrapped array of linked lists; it never changes.
	private int curSize; //initially equal to vertexCount; changes to reflect deletions and insertions on the Adjacency List.
	private LinkedList[] list; //contains all of the linkedlists that make up the adjacency list
	private String status;
	private int[] deletionArray; //keeps track of whether or not a certain node is deleted
	private final int INFINITY = 2147483647; //max integer #, used for initial values in several of the graph algorithms.
	private ArrayList<String> pathList; //global arraylist for holding calculated paths
	private int[] marked; //used for determining connectivity of the network.


	//error messages
	private final String errIO = "I/O Exception! System Exiting...";
	private final String errListOutOfBounds = "\nAttempted to insert vertex into an insufficiently sized Adjacency List!\n";
	private final String errInvalidEdgeWeight = "\nInvalid edge specifications; unable to insert edge.\n";
	private final String errDeletedNode = "Unable to modify or delete edge; one or both edge vertices currently in a deleted state\n";
	private final String errInvalidVertex = "FATAL ERROR: vertex not contained within list!\n";
	private final String errNodeDeleted = "\nERROR: Unable to calculate network path; a network node may be temporarily down, or the path may not exist.\n";
	private final String errNodeDown = "\nERROR: Unable to contact node; the network node may be temporarily down, or the path may not exist.\n";
	private final String errNodeUp = "\nERROR: Unable to initiate automatic repair procedures while network node reporting healthy status!\n";


	
    //-------------------------------------------------------------------------------------------------------------	
	//AdjacencyList Constructors:
    //-------------------------------------------------------------------------------------------------------------


    //-------------------------------------------------------------------------------------------------------------
    // ~~AdjacencyList(int v)~~
    //	-constructor that takes a number of vertices as an argument.
    //	-NOTE that this is NOT the number of ACTUAL vertices in this graph, but the maximum number it can hold; that is, 
    //   -the length of the <list> array of Linked Lists.
    //-------------------------------------------------------------------------------------------------------------
	public AdjacencyList(int v) 
	{
		this.vertexCount = v;
		curSize = vertexCount;
		deletionArray = new int[vertexCount];
		this.list = new LinkedList[vertexCount];
		for (int i = 0; i<=list.length-1; i++)
		{
			deletionArray[i] = 1;
			this.list[i] = new LinkedList();
		}
	}


    //-------------------------------------------------------------------------------------------------------------
    //  ~~AdjacencyList(BufferedReader r)~~
    //	-this constructor takes in a BufferedReader opened on a file containing the specifications of the graph to be built.
    //	-It initializes all global vars, and builds the adjacency list structure.
	//
    //-------------------------------------------------------------------------------------------------------------
	public AdjacencyList(BufferedReader r) 
	{
		ArrayList<String> specList = new ArrayList<String>(); //specList will contain each line of text read from the input file in its subsequent indices. 
		String input = null;
		String line;
		try
		{
			while ((input = r.readLine())!=null) //read in graph specs from file
			{
				line = new String(input); //call to remove spaces from current line of text
				specList.add(line);
			} //specList now possesses the following data: index 0 contains the number of vertices in the graph, index 1 contains the number of edges, and index 2 through specList.size()-1 contains all vertex/edge combinations.
		}
		catch (IOException e)
		{
			e.printStackTrace();
			perror(errIO);
		}
		this.vertexCount = Integer.parseInt(specList.get(0));
		this.edgeCount = Integer.parseInt(specList.get(1));
		curSize = vertexCount;
		deletionArray = new int[this.vertexCount]; 
		//edgeList = new ArrayList<Edge>();
		this.list = new LinkedList[this.vertexCount];
		for (int i = 0; i <= vertexCount-1; i++) //for each vertex i in the graph, that vertex's number is the index of a linked list containing i's neighbors
		{
			deletionArray[i] = 1;
			this.list[i] = new LinkedList(i, specList); //the array index corresponding TO that vertex contains a LinkedList holding neighbors of i
		}
		status="Connected";
	}
	


    //-------------------------------------------------------------------------------------------------------------
	//AdjacencyList methods:
	//-------------------------------------------------------------------------------------------------------------



    //-------------------------------------------------------------------------------------------------------------
    // ~~contains(int vertex)~~
    //	-this method returns true if the vertex is contained in the adjacency list, and
    //	-returns false if it is not.
	//
    //-------------------------------------------------------------------------------------------------------------
	public boolean contains(int vertex)
	{
		for(int i = 0; i <= this.list.length-1; i++)
		{
			Node tempNode = list[i].head;
			while (tempNode!=null)
			{
				int v0 = tempNode.getData().someVertex();
				int v1 = tempNode.getData().adjacentVertex(v0);
				if ((v0==vertex) || (v1==vertex)) return true;
				tempNode=tempNode.nextNode;
			}
		}
		return false;
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~contains(edge e)~~
    //	-this method returns true if the edge is contained in the adjacency list, and
    //	-returns false if it is not.
	//
	//-------------------------------------------------------------------------------------------------------------
	public boolean contains(Edge e)
	{
		for (int i = 0; i <= this.list.length-1; i++)
		{
			if (this.list[i].find(e)==true) return true;
		}
		return false;
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~insert()~~
    //	-this void function handles the insertion of new edges into the graph
    //	
	//
	//-------------------------------------------------------------------------------------------------------------
	public void insert(Edge newEdge)
	{
		int v0 = newEdge.someVertex();
		int v1 = newEdge.adjacentVertex(v0);
		if ((v0 > this.list.length-1)||(v1 > this.list.length-1)) throw new java.lang.ArrayIndexOutOfBoundsException(errListOutOfBounds);
		if ((this.contains(v0)!=true) || this.contains(v1)!=true) //as long as at least one of the included vertices is NOT already in the adjacency list, you're good to go.
		{
			//System.out.println("Inserting new edge: (" + newEdge.getVertexA() + ", " + newEdge.getVertexB() + ", " + newEdge.getWeight() + ")");
			this.list[newEdge.getVertexA()].insert(newEdge);
			this.list[newEdge.getVertexB()].insert(newEdge.reverse());
		}
		else if (this.contains(newEdge) == false) //then both vertices exist in the graph BUT an edge does NOT exist between them. so create it.
		{	
			//System.out.println("Creating a path between two pre-existing vertices " + newEdge.getVertexA() + ", " + newEdge.getVertexB());
			this.list[newEdge.getVertexA()].insert(newEdge);
			this.list[newEdge.getVertexB()].insert(newEdge.reverse());
		}
		else
		{
			//System.out.println("INSERT FAILED: edge between vertices " + v0 + " and " + v1 + " already exists!");	
		} 
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~report()~~
    //	-this method displays information about each individual node currently active in the network.
    //	
	//
	//-------------------------------------------------------------------------------------------------------------
	public void report() //display detailed information on each Node 
	{
		System.out.println();
		if (this.isConnected()==true) System.out.println("\nNetwork Status: CONNECTED.\n");
		else System.out.println("\nNetwork Status: DISCONNECTED\n");
		/*System.out.println();
		for (int i = 0; i <= list.length-1; i++) //for each index in list
		{
			list[i].display(); //call each Linked List's display() method
		}*/

		this.view();
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~isConnected()~~
    //	-Depth-First Search, by definition, visits all vertices in a connected graph. If, after DFS has completed, not all nodes have been visited, then proof by contradiction yields that the graph is disconnected.
	//  -this method calls the recursive Depth-First-Search function below, which in turn determines the connectivity fo the graph.
	//-------------------------------------------------------------------------------------------------------------
	public boolean isConnected()
	{
		int curLocation = 0;
		System.out.println("Current number of vertices in graph: " + this.curSize);
		marked = new int[this.vertexCount];
		for (int j = 0; j <= this.vertexCount-1; j++)
		{
			marked[j] = 0; //preset is 0; when visited each node's respective index gets set to 1.
			if (deletionArray[j]==0) marked[j] = 1;
		}
		for (int i = 0; i <= this.list.length-1; i++) //for loop iterates over all vertices until it finds a list that is not empty. Our arbitrary starting location for the algorithm is the head of this list.
		{
			if (this.list[i].size > 0)
			{
				curLocation = this.list[i].head.getData().getVertexA();
				//System.out.println("Current starting list size: " + this.list[curLocation].size);
				break;
			}
		}

		DFS(curLocation); //call DFS on the starting point

		boolean connected = true;
		for (int i = 0; i<=this.marked.length-1; i++) //IF any indices of marked[] are NOT 1, then the graph is not connected, as by definition Depth-First Search visits all vertices within the connected portion of a graph, thus a connected graph would yield an array of all 1's.
		{
			if (marked[i] != 1)
			{
				connected = false;
				break;
			}
		}
		//System.out.println(Arrays.toString(marked));
		return connected;

	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~DFS()~~
    //	-this method uses Depth-First-Search to recursively search the graph.
	//  -it is called by the isConnected() function above.
	//-------------------------------------------------------------------------------------------------------------
	private void DFS(int curLocation)
	{
		marked[curLocation] = 1;
		Node curNode = this.list[curLocation].head;
		while (curNode != null)
		{
			if (this.marked[curNode.getData().adjacentVertex(curLocation)] != 1) //if the adjacent node has yet to be visited
			{
				DFS(curNode.getData().adjacentVertex(curLocation)); //visit it!
			}
			curNode = curNode.nextNode;
		}
		//System.out.println("Falling out of void...");
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~prims()~~
    //	-this method calculates the Minimum Spanning Tree of the graph via my implementation of Prim's Algorithm.
    //	
	//
    //-------------------------------------------------------------------------------------------------------------
	public void prims()
	{
		int MSTsize = 0;
		
		AdjacencyList MST = new AdjacencyList(vertexCount); //Adjacency List has <vertexCount> possible vertices.
		int[] MSTVertices = new int[vertexCount];
		int[] GVertices = new int[vertexCount];

		for(int i = 0; i <= vertexCount-1; i++)
		{
			GVertices[i] = 1; //initialize G's vertices as all utilized.
			MSTVertices[i] = 0; //initialize MST as all uninserted.
		}
		int curLocation = 0;
		for (int i = 0; i <= this.list.length-1; i++) //for loop iterates over all vertices until it finds a list that is not empty. Our arbitrary starting location for the algorithm is the head of this list.
		{
			//System.out.println("Linked List " + i + " size: " + this.list[i].size);
			if (this.list[i].head != null)
			{
				curLocation = this.list[i].head.getData().getVertexA();
				//System.out.println("Starting location: " + curLocation);
				break;
			}
		}
		GVertices[curLocation] = 0; //remove from GVertices the initial starting location.
		MSTVertices[curLocation] = 1; //and add it to the MST.
		MSTsize = 1;
		while(true)
		{
			Edge cheapestEdge = new Edge(this.findMinimumEdge(GVertices, MSTVertices)); //find the lowest-weighted edge connecting a vertex in the tree with a vertex NOT in the tree.

			if (cheapestEdge.getWeight() == INFINITY)
			{
				//System.err.println("FATAL ERROR: Edge of Length INFINITY has been added to graph! Contact your local physicist if the problem persists.");
				break;
			} 
			if (MSTVertices[cheapestEdge.getVertexA()]==0) MSTsize++; //if vertexA has yet to be added, increment total size by 1.
			if (MSTVertices[cheapestEdge.getVertexB()]==0) MSTsize++; //if vertexB has yet to be added, increment total size by 1.

			GVertices[cheapestEdge.getVertexA()] = 0;
			MSTVertices[cheapestEdge.getVertexA()] = 1;
			GVertices[cheapestEdge.getVertexB()] = 0;
			MSTVertices[cheapestEdge.getVertexB()] = 1;

			MST.insert(cheapestEdge);
			if (MSTsize==this.curSize) break; //ending condition.
		}
		MST.view();
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~findMinimumEdge()~~
    //	-this method finds the lowest-weighted edge connecting a vertex in the tree with a vertex NOT in the tree.
    //  -It is used by my Prim's implementation above
    //	-It does not operate on deleted nodes, and will output an error message should this be attempted via the above method
	//
	//-------------------------------------------------------------------------------------------------------------
	private Edge findMinimumEdge(int[] graphVertices, int[] treeVertices) 
	{
		Edge cheapestEdge = new Edge(1, 1, INFINITY);
		Edge innerCheapestEdge = new Edge(1, 1, INFINITY);	
		for (int i = 0; i <= this.list.length-1; i++)
		{
			if ((treeVertices[i] == 1) && (this.list[i].size > 0)) //if the tree does contain the node represented by this linked list AND the linked list is not empty
			{	
				//innerCheapestEdge = new Edge(1, 1, INFINITY);
				//cheapestEdge = new Edge(1, 1, INFINITY);
				Node tempNode = this.list[i].head; 	
				while (tempNode!=null)
				{
					if (graphVertices[tempNode.getData().getVertexB()] == 1) //find the CHEAPEST edge that connects to a vertex in G.
					{
						if (tempNode.getData().getWeight() < innerCheapestEdge.getWeight())
						{
							innerCheapestEdge = tempNode.getData();
						}
					}
					tempNode = tempNode.nextNode;
				}
				if (innerCheapestEdge.getWeight() < cheapestEdge.getWeight())
				{
					cheapestEdge = innerCheapestEdge;
				}
			}
		}
		return cheapestEdge;
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~djikstras()~~
    //	-This method uses Djikstra's Shortest Path Algorithm to find the lowest-latency route between two arbitrary points in the network
    //	-It does not operate on deleted nodes, and will return an error message should this be attempted.
    //
	//
	//-------------------------------------------------------------------------------------------------------------
	public void djikstras(int start, int end) //
	{
		if ((this.deletionArray[start]==1) && (this.deletionArray[end]==1))
		{
			int curLocation = start;
			int[] foundArray = new int[vertexCount];
			String[] pathArray = new String[this.vertexCount];
			int[] distance = new int[vertexCount];
			for (int i = 0; i <=vertexCount-1; i++) //initialize array values
			{
				pathArray[i] = new String(Integer.toString(start) + " ");
				foundArray[i] = 0;
				distance[i] = INFINITY;
			}
			while (true) //we must break from within to end the loop
			{
				Node temp = this.list[curLocation].head;
				while (temp!=null)
				{
					if((temp.getData().getWeight() + distance[curLocation]) < distance[temp.getData().adjacentVertex(curLocation)]) //if (length of current edge being tested) + (length of current path) is CHEAPER than (current recorded path length)
					{
						distance[temp.getData().adjacentVertex(curLocation)] = temp.getData().getWeight() + distance[curLocation]; //update distance array
						pathArray[temp.getData().adjacentVertex(curLocation)] = new String(pathArray[curLocation] + Integer.toString(temp.getData().adjacentVertex(curLocation)) + " ");
					}
					temp = temp.nextNode;
				}
				if (curLocation==end) break; //end condition
				foundArray[curLocation]=1;
				curLocation = this.list[curLocation].getCheapestEdge(foundArray).adjacentVertex(curLocation); //jump to the cheapest available edge's adjacent vertex.
			}
			//System.out.println("Path spans along vertices: " + pathArray[end]);
			
			int[] pathVertices = getInts(pathArray[end]);
			AdjacencyList shortestPath = new AdjacencyList(this.vertexCount);

			for (int j = 0; j <= pathVertices.length-2; j++)
			{
				int edgeEnd = j+1;
				
				shortestPath.insert(this.list[pathVertices[j]].getEdge(pathVertices[edgeEnd])); //returns the Edge object spanning between node j and j+1
			}
			//shortestPath.report();
			System.out.println("\nLowest Latency Path Between Node " + start + " and Node " + end + " Summary: \n");
			shortestPath.view();
		}
		else
		{
			System.err.println(errNodeDeleted);
		}
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~disableNode()~~
   	//  -removes an edge from the graph.
    //	-This function deletes the requested edge as well as it's reversed twin, via the Edge.reverse() method.
    //
	//
	//-------------------------------------------------------------------------------------------------------------
	public void deleteEdge(Edge e)
	{
		if (this.contains(e)==true)
		{
			this.list[e.getVertexA()].deleteNode(e.getVertexB()); //delete requested edge a----->b
			this.list[e.getVertexB()].deleteNode(e.getVertexA()); //and it's opposite	  b----->a
		}
		else
		{
			System.err.println("UNABLE TO DELETE EDGE!");
		}

	}



	//-------------------------------------------------------------------------------------------------------------
    // ~~distinctPaths()~~
   	//  -calculates all valid paths between int start and int end with constraining variable int max determining the maximum path weight.
    //	-Stores paths in String variables, which are in turn stored in global ArrayList pathList, and then output via a for loop.
    //  -BUG NOTE: CRASHES WHEN YOU ENTER A MAXIMUM WEIGHT WHICH IS SMALLER THAN ANY WEIGHT IN THE GRAPH
	//-------------------------------------------------------------------------------------------------------------
	public void distinctPaths(int start, int end, int max)
	{
		if ((this.deletionArray[start]==1) && (this.deletionArray[end]==1))
		{
			System.out.println("Calculating all possible distinct paths between " + start + " and " + end + " with maximum length " + max + ". \n");
			pathList = new ArrayList<String>();

			int d = 0;
			//String curPath = new String(" " + start);
			String curPath = new String();

			searchPaths(start, end, d, curPath, max);
			
			if (pathList.get(0).charAt(0)==' ') //remove space at beginning of string to avoid confusing the string->int conversion method
			{
				String temp = new String(pathList.get(0));
				pathList.remove(0);
				pathList.add(temp.substring(1, temp.length()));
			}
		
			int[] paths;
			for (int i = 0; i <= pathList.size()-1; i++)
			{
				System.out.println("\nPath # " + (i+1) + ": ");
				paths = getInts(pathList.get(i)); //convert path from it's String form into an array of ints

				for (int j = 0; j <= paths.length-2; j++)
				{
					System.out.print("(" + paths[j] + ")---------->(" + paths[j+1] + ")");
				}
				System.out.println();
			}
		}
		else
		{
			System.err.println(errNodeDeleted);
		}


	}

   

    //-------------------------------------------------------------------------------------------------------------
    // ~~searchPaths()~~
   	//   -A recursive, backtracking method that calculates all valid paths between points A and B.
    //	-Stores paths in String variables, which are in turn stored in global ArrayList pathList if the path is valid.
    //
	//-------------------------------------------------------------------------------------------------------------
	private void searchPaths(int curLocation, int endLocation, int curDistance, String curPath, int maxLen)
	{

			//System.out.println("Now entering searchPaths(). Current start location: " + curLocation + ", current distance of path: " + curDistance + ", current path: " + curPath);
			if (curDistance > maxLen) 
			{
				return;
			}

			if (curLocation == endLocation)
			{
				//System.out.println("PATH FOUND!!");
				pathList.add(new String(curPath + curLocation + " "));
				return;
			}
			Node curNode = this.list[curLocation].head;
			while (curNode != null)
			{
				//System.out.println("CurNode value: " + curNode.getData().getVertexA() + ", " + curNode.getData().getVertexB());
				if (curPath.contains(Integer.toString((Integer)curNode.getData().adjacentVertex(curLocation))) != true)
				{
					searchPaths(curNode.getData().adjacentVertex(curLocation), endLocation, (curDistance + curNode.getData().getWeight()), new String(curPath + curLocation + " "), maxLen); //recursive call to searchPaths, but with new bounds
				}
				curNode = curNode.nextNode;
			}
			return;
	}



	//-------------------------------------------------------------------------------------------------------------
    // ~~disableNode()~~
    //	-This function takes in a single int param that represents the vertex to be brought down. 
    //	-Should the vertex not exist, nothing is done.
    //
	//
	//-------------------------------------------------------------------------------------------------------------
	public void disableNode(int node) //deletes all edges connected to int node in the graph.
	{
		if (deletionArray[node]==1)
		{
			System.out.println("\nWARNING!\nNetwork Node " + node + " is no longer responding. Attempting automatic repair procedures. \n(This event has been logged)\n");
			this.curSize--;
			this.deletionArray[node] = 0; //mark the node as deleted in the array
			int[] deletionArray = this.list[node].disable(); //disable returns an array of adjacent vertices such that all edges containing (node, deletionArray[i]) must be removed
			for (int i = 0; i <= deletionArray.length-1; i++)
			{
				//System.out.println("Deleting Edge (" + deletionArray[i] + ", " + node + ")");
				this.list[deletionArray[i]].deleteNode(node);
			}
			this.view();
		}
		else
		{
			System.err.println(errNodeDown);
		}	
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~restoreNode()~~
    //   restores all edges connected to int node in the graph
    //	-This function takes in a single int param that represents the vertex to be repaired. 
    //	-Should the vertex not exist, nothing is done.
    //
	//
	//-------------------------------------------------------------------------------------------------------------
	public void restoreNode(int node) //
	{
		System.out.println("\nInitiating automatic repair procedures on Node " + node + "...\n");
		if (deletionArray[node]==0)
		{
			this.curSize++;
			this.deletionArray[node] = 1;
			for (int i = 0; i <= this.list.length-1; i++) //for each element of the adjacency list
			{
				for (int j = 0; j <= this.list[i].edgeGraveYard.size()-1; j++)  //iterate through the contents of the graveyard arraylist.
				{
					if (this.list[i].edgeGraveYard.get(j).connectsNode(node)) //if the edge graveyard contains an edge connecting to node
					{
						if ((deletionArray[this.list[i].edgeGraveYard.get(j).getVertexA()] == 1) && (deletionArray[this.list[i].edgeGraveYard.get(j).getVertexB()] == 1)) //IF neither vertex in the edge is still classified as deleted
						{
							this.insert(this.list[i].edgeGraveYard.get(j)); //re insert the edge back into the graph
						}
					}
				}
			}
			this.view();
			System.out.println("\nNode " + node + " has been restored!\n");
		}
		else
		{
			System.err.println(errNodeUp);
		}

	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~modifyWeight()~~
    //	-this void function iterates through the Adjacency List in search of the target edge, and modifies/deletes/creates the edge as per the spec sheet.
    //	-it takes in two integers v0 and v1, as well as a third int w. If v0 and v1 represent the vertices of an already-created edge, w replaces that edge's current value. If w is negative, however, that edge is deleted.
    //	-Should the edge NOT exist, it is created. If the edge has a negative value, nothing is done. 
    //	-NOTE: Edges cannot be deleted, inserted or otherwise modified should either node be down.
	//
	//-------------------------------------------------------------------------------------------------------------
	public void modifyWeight(int v0, int v1, int w) 
	{
		if ((this.deletionArray[v0] == 1) && (this.deletionArray[v1] == 1)) //if neither vertex is currently deleted.... 
		{
			Edge tempEdge = new Edge(v0, v1, w);
			if (this.contains(tempEdge)==true)
			{
				System.out.println("Modifying edge (" + v0 + ", " + v1 + ", " + w + ") ");
				if (w <= 0) //delete the edge
				{
					deleteEdge(tempEdge);
				}
				else if (w > 0) //change the weight
				{
					Node tempNode = this.list[v0].head;
					while (tempNode != null)
					{
						if ((tempNode.getData().getVertexA() == v0) && (tempNode.getData().getVertexB() == v1)) //edge has been found
						{
							tempNode.getData().setWeight(w);
						}
						tempNode = tempNode.nextNode;
					}
					tempNode = this.list[v1].head;
					while (tempNode != null)
					{
						if ((tempNode.getData().getVertexA() == v1) && (tempNode.getData().getVertexB() == v0)) //inverse edge has been found.
						{
							tempNode.getData().setWeight(w);
						}
						tempNode = tempNode.nextNode;
					}
				}
			}
			else if (w > 0)  //create new edge
			{
				System.out.println("Creating edge (" + v0 + ", " + v1 + ", " + w + ") ");
				this.insert(tempEdge);
			}
			else //can't add negative edge.
			{
				System.err.println(errInvalidEdgeWeight);
			}
		}
		else System.err.println(errDeletedNode);
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~view()~~
    //	-this void function is used to display the current state of a graph. It displays the information on a per-vertex basis; that is, it displays the vertex and each edge
    //	-connected to that vertex.
    //
	//-------------------------------------------------------------------------------------------------------------
	public void view()
	{
		//System.out.println("Current Graph State: \n");
		for (int i = 0; i <= this.list.length-1; i++)
		{
			System.out.print("	Links connected to Node " + i + " . . . . . . . . . . . . . . : ");
			Node tempNode = this.list[i].head;
			while (tempNode!=null)
			{			
				System.out.print(" (" + tempNode.getData().getVertexA() + ") ---" + tempNode.getData().getWeight() + "---> (" + tempNode.getData().getVertexB() + ")");
				tempNode = tempNode.nextNode;
			}
			System.out.println();
			
		}
		System.out.println();
	}



    //-------------------------------------------------------------------------------------------------------------
    // ~~perror()~~
    //	-This function handles the display of error messages via standard error and exits the program
	//
	//-------------------------------------------------------------------------------------------------------------
	private final void perror(final String msg)
	{
		System.err.println(msg);
		System.exit(-1);
	}



	//-------------------------------------------------------------------------------------------------------------
    // ~~getInts()~~
    //	-This function takes in a single String parameter, and converts it into an array of integers. This is used by several algorithms who use
    //	-strings to record their results, such as my implementation of Djikstra's Algorithm.
    //
	//
	//-------------------------------------------------------------------------------------------------------------
	static int[] getInts(String s)
	{
		String[] strArray = s.split(" ");
		int[] intArray = new int[strArray.length];
		for (int i = 0; i <= strArray.length-1; i++)
		{
			intArray[i] = Integer.parseInt(strArray[i]);
		}
		return intArray;
	}



	//-------------------------------------------------------------------------------------------------------------
	//AdjacencyList Internal Classes:
	//-------------------------------------------------------------------------------------------------------------



	//-------------------------------------------------------------------------------------------------------------
	// private class LinkedList
	// implements a Singly Linked List data structure such that each index in AdjacencyList field 'list' contains such a construct 
	// whose size & # of elements is determined by the neighbors of the graph vertex represented by list[i]
	//-------------------------------------------------------------------------------------------------------------
	private class LinkedList 
	{
		//LinkedList fields:
		public Node head;
		private int size = 0;
		private boolean status;
		private ArrayList<Edge> edgeGraveYard; //this data structure maintains a list of edges that are temporarily brought down. For the purposes of this network simulation, deleted edges are
											   //not actually deleted, but simply removed from the link list and subsequently stored in this ArrayList so that they can be retrieved.


		//-------------------------------------------------------------------------------------------------------------
		//LinkedList constructors:
		//-------------------------------------------------------------------------------------------------------------



		public LinkedList()
		{
			edgeGraveYard = new ArrayList<Edge>();
		}



		//-------------------------------------------------------------------------------------------------------------
		//	this constructor takes in an integer, representing a vertex, and an ArrayList containing data about the graph. 
		//	it iterates over the (vertex vertex weight) data combinations of the data structure and adds LinkedList nodes for every neighbor of vertex n, as per the definition of an Adjacency List
		//-------------------------------------------------------------------------------------------------------------
		public LinkedList(int n, ArrayList<String> specList)
		{
			this.status=true; //Represents current vertex status in the network. all vertices initially initialized to true
			edgeGraveYard = new ArrayList<Edge>();
			int numDigitsV0, numDigitsV1, numDigitsWeight;
			String temp;
			for(int i = 2; i <= specList.size()-1; i++) //for each entry in specList, gather the vertices in each row, and see if either vertex is equal to the current value of n. n represents the current value of the index of the main array of linkedlists being checked.
			{
				char c = specList.get(i).charAt(0); //c = to the first digit
				int index = 0;
				while (c!=' ') //while c is NOT whitespace
				{
					index++; //increase index repeatedly until c equals a whitespace
					c = specList.get(i).charAt(index);
				} //index now gives us the # of digits of the first vertex 
				numDigitsV0=index;

				index++;
				c = specList.get(i).charAt(index); //move one past whitespace
				while (c!=' ') //while c is NOT whitespace
				{
					index++; //increase index repeatedly until c equals a whitespace
					c = specList.get(i).charAt(index);
				} //index - (numDigitsv0+1) now gives us the # of digits of the second vertex 
				numDigitsV1=(index-(numDigitsV0+1));

				index++;
				c = specList.get(i).charAt(index); //move one past whitespace
				while (index!=(specList.get(i).length()-1)) //while c is NOT whitespace
				{
					index++; //increase index repeatedly until c equals a whitespace
					c = specList.get(i).charAt(index);
				} //index - ((numDigitsv0+1) + (numDigitsv1+1)) now gives us the # of digits of the second vertex 
				numDigitsWeight=index-((numDigitsV0+1)+(numDigitsV1+1));

				temp = new String(specList.get(i).substring(0, numDigitsV0)); //temp = string version of v0
				int v0 = Integer.parseInt(temp);

				temp = new String(specList.get(i).substring(numDigitsV0+1, ((numDigitsV0) + (numDigitsV1))+1));
				int v1 = Integer.parseInt(temp);

				temp = new String(specList.get(i).substring(((numDigitsV0+1) + (numDigitsV1+1)), ((numDigitsV0+1) + ((numDigitsV1+1)) + (numDigitsWeight+1))));
				int w = Integer.parseInt(temp);

				if (v0==n)
				{
					this.insert(new Edge(v0, v1, w)); //if the first vertex in the line of text is equal to the current value of n, then an edge exists between n and the second vertex
				} 
				else if (v1==n) 
				{
					this.insert(new Edge(v1, v0, w));
				}
			} //NOW, the list array contains a linked list that contains each connecting edge to vertex n
		}
		


		//-------------------------------------------------------------------------------------------------------------
		//LinkedList methods:
		//-------------------------------------------------------------------------------------------------------------



		//-------------------------------------------------------------------------------------------------------------
		//	void deleteNode()
		//	removes the node from the linked list. Note that instead of being permanently deleted,
		//	the contained Edge object is stored in the edgeGraveYard array so that it can be easily retrieved.
		//-------------------------------------------------------------------------------------------------------------
		public void deleteNode(int b) 
		{
			Node curNode = this.head;
			Node prevNode = curNode;
			while (curNode != null)
			{
				if (curNode.getData().getVertexB()==b) //if the edge has been located, proceed with deletion...
				{
					if ((curNode.getData().getVertexA()==this.head.getData().getVertexA()) && (curNode.getData().getVertexB()==this.head.getData().getVertexB())) //if we're deleting the head node...
					{
						edgeGraveYard.add(new Edge(curNode.getData()));
						this.head = curNode.nextNode; //point the head ptr to the next node

						curNode = null; //sever the connection.
					}
					else
					{
						edgeGraveYard.add(new Edge(curNode.getData())); //store the linked list in edgeGraveYard prior to removal from list to save it from java's garbage collector
						prevNode.nextNode = curNode.nextNode; //remove references to curNode. the node is now deleted.

						curNode = null;
					}
					this.size--;
					break;
				}
				prevNode = curNode;
				curNode = curNode.nextNode;
			}
		}



		//-------------------------------------------------------------------------------------------------------------
		// getCheapestEdge()
		// returns the cheapest edge in this linked list.
		// uses a standard LL iterator node ptr.
		//-------------------------------------------------------------------------------------------------------------
		public Edge getCheapestEdge()
		{
			Node tempNode = this.head;
			int cheapestEdge = 100;
			int count = 0;
			int cheapestIndex = 0;
			while (tempNode!=null) //iterates over the nodes in the linked list, searching for the one with the cheapest adjacent edge
			{
				int curEdge = tempNode.getData().getWeight();
				if (curEdge < cheapestEdge)
				{
					cheapestEdge = curEdge;
					cheapestIndex = count;	
				}
				count++; 
				tempNode=tempNode.nextNode;
			}
			int j = 0;
			Node iterator = this.head;

			while (j != (cheapestIndex))
			{
				iterator=iterator.nextNode;
				j++;
			}
			return new Edge(iterator.getData());
		}



		//-------------------------------------------------------------------------------------------------------------
		// getCheapestEdge(int[])
		// specialized variant of the above method that accepts an array of integers set to either 1 or 0. 
		// the array represents a list of vertices in a graph and whether they are present or not.
		//-------------------------------------------------------------------------------------------------------------
		public Edge getCheapestEdge(int[] intArray) 
		{
			Node tempNode = this.head;
			int cheapestEdge = INFINITY;
			int count = 0;
			int cheapestIndex = 0;
			while (tempNode!=null) //iterates over the nodes in the linked list, searching for the one with the cheapest adjacent edge
			{
				if ((intArray[tempNode.getData().getVertexA()]!=1) || (intArray[tempNode.getData().getVertexB()]!=1))
				{
					int curEdge = tempNode.getData().getWeight();
					if ((curEdge < cheapestEdge))
					{
						cheapestEdge = curEdge;
						cheapestIndex = count;	
					}	
				}
				count++; 
				tempNode=tempNode.nextNode;
			}
			int j = 0;
			Node iterator = this.head;
			while (j != (cheapestIndex))
			{
				iterator=iterator.nextNode;
				j++;
			}
			return new Edge(iterator.getData());
		}



		//-------------------------------------------------------------------------------------------------------------
		// getEdge(int)
		// returns the edge in this linked list whose second vertex is the passed-in int parameter.
		// 
		//-------------------------------------------------------------------------------------------------------------
		public Edge getEdge(int vertex_b) //returns the edge whose vertex 'b' value equals that of the parameter.
		{
			//System.out.println("Searching for " + vertex_b + " in ");
			//this.display(); 
				Node tempNode = this.head;
				while(tempNode!=null)
				{
					if (tempNode.getData().getVertexB()==vertex_b) 
					{	
						return tempNode.getData();
					}
					else tempNode = tempNode.nextNode;
				}
				perror(errInvalidVertex);
				//System.exit(-1);
				return new Edge(0, 0, 0); //to keep Java from complaining.

			//throw new NullPointerException("Vertex not contained within list!");
		}



		public boolean getStatus()
		{
			return this.status;
		}



		public void setStatus(boolean b)
		{
			this.status=b;
		}



		//-------------------------------------------------------------------------------------------------------------
		// void display()
		// when called, this function iterates over the list, causing each contained node to call the edge contained within itself to display relevant data.
		// 
		//-------------------------------------------------------------------------------------------------------------
		private void display()
		{
			Node temp = this.head;
			while (temp!=null)
			{
				temp.display();
				temp = temp.nextNode;
			}
		}



		//-------------------------------------------------------------------------------------------------------------
		// int[] disable()
		// this function iterates over the entire list, storing each successive node in the edgeGraveYard ArrayList prior to deletion.
		// it returns an array of integers, which are the adjacent nodes for each edge that was deleted. This allows the calling function to ensure that the reverse representations
		// of each edge are also deleted properly.
		//-------------------------------------------------------------------------------------------------------------
		private int[] disable()
		{
			int[] vertexArray = new int[this.size]; //array for holding the adjacent vertices of each node prior to deletion so that their reverse representations can ALSO be deleted.
			Node tempNode = this.head;
			int i = 0;
			while(tempNode!=null)
			{
				edgeGraveYard.add(new Edge(tempNode.getData())); //add the edge contained within the node to "cryo-storage".
				vertexArray[i] = tempNode.getData().getVertexB();
				tempNode = tempNode.nextNode;
				i++;
			}
			this.head=null; //cuts the entire node string loose for garbage collection.
			this.size=0;
			return vertexArray; //returns the list of adjacent nodes to the calling Adjacency List method to ensure deletion of reverse edge representations.
		}



		//-------------------------------------------------------------------------------------------------------------
		// void insert(edge e)
		// this method inserts the edge into a node at the head of list.
		// 
		//-------------------------------------------------------------------------------------------------------------
		private void insert(Edge e)
		{
			if (this.find(e)==false)
			{
				Node newNode = new Node(e);
				if (this.size==0) 
				{
					this.head=newNode;
				}
				else
				{
					newNode.nextNode = this.head;
					this.head = newNode;
				}
				this.size++;
			}
		}



		//-------------------------------------------------------------------------------------------------------------
		// void find(edge e)
		// returns true if edge is located in a node in the linked list
		// returns false otherwise.
		//-------------------------------------------------------------------------------------------------------------
		private boolean find(Edge e) //returns true if n located in LinkedList, returns false if n not found.
		{
			Node temp = this.head;
			while (temp!=null)
			{
				if ((e.getVertexA()==temp.getData().getVertexA()) && (e.getVertexB()==temp.getData().getVertexB())) return true;
				else temp=temp.nextNode;
			}
			return false;
		}
	}// end linkedlist class



	//-------------------------------------------------------------------------------------------------------------
	// private class Node
	// implements the node object used by a linked list.
	// contained within each node's Data field is an Edge.
	// is external in regards to the linked list to allow for node traversal algorithms to be easily written for the linked list but outside of it.
	//-------------------------------------------------------------------------------------------------------------
	private class Node 
	{
		//Node fields:
		private Edge data;
		private Node nextNode;
		
		//Node constructors:
		public Node(Edge e)
		{
			this.data=e;
		}
		
		//Node methods:
		public void display()
		{
			data.display();
		}
		public Edge getData()
		{
			return this.data;
		}
	} //end node class



	//-------------------------------------------------------------------------------------------------------------
	// private class Edge
	// implements an Edge object to simulate an edge in a directed weighted graph. Possesses a weight, two vertices,
	// and even a reverse() function that, when called, returns the same edge but with Vertex A and B switched. This is integral
	// to the organization of the adjacency list.
	//-------------------------------------------------------------------------------------------------------------
	private class Edge //simulates an edge in an undirected weighted graph
	{
		//Edge fields:
		private int weight;
		private int Node_0;
		private int Node_1;
		private boolean state;

		//Edge constructors:
		public Edge(int a, int b, int edgeWeight)
		{
			state = true;
			this.Node_0=a;
			this.Node_1=b;
			this.weight=edgeWeight;
		}
		public Edge(Edge e)
		{
			this(e.Node_0, e.Node_1, e.weight);
		}

		//Edge methods:
		public void setWeight(int w)
		{
			this.weight=w;
		}
		public int getVertexA()
		{
			return Node_0;
		}
		public int getVertexB()
		{
			return Node_1;
		}

		public Edge reverse() //returns a new Edge object whose vertices are reversed, and whose weight is the same as the parent object. This method is utilized when inserting new edges into the Adjacency List
		{
			return new Edge(Node_1, Node_0, weight);
		}
		public void display() //displays status
		{
			System.out.println (
									   "Link from Node " + Node_0 + " to Node " + Node_1 + ":\n" 
									 + "\n   Link Status  . . . . . . . . . . . . . : " + curState(state) + "\n"
									 + "   Node " + Node_1 + " . . . . . . . . . . . . . . . . : " + curState(list[Node_1].getStatus()) + "\n"
									 + "   Node " + Node_0 + " . . . . . . . . . . . . . . . . : " + curState(list[Node_0].getStatus()) + "\n"
									 + "   Connection Latency . . . . . . . . . . : " + weight + "\n"
							   );
		}
		public int someVertex() //returns a random vertex.
		{
			int num;
			Random rand = new Random();
			num = rand.nextInt();
			num = num % 2;
			if (num==0) return Node_0;
			else return Node_1;
		}
		public int adjacentVertex(int v) //returns the vertex opposite of parameter v
		{
			if (v==Node_0) return Node_1;
			else return Node_0;
		}
		public int getWeight()
		{
			return this.weight;
		}
		public boolean isActive()
		{
			return this.state;
		}
		public void setState(boolean b)
		{
			this.state=b;
		}
		public boolean connectsNode(int node)
		{
			if ((Node_0==node) || (Node_1==node))
			{
				return true;
			}
			else return false;
		}
		private String curState(boolean b)
		{
			if (b==true) return new String("Active");
			else return new String("Inactive");
		}
	} //end edge class
} //end adjacencylist class
