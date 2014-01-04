/*
 *@Programmer: Tyler Raborn
 *@Assignment: Project 5 - Network Graph
 *@Description: This class handles the primary user and file I/O for building the network graph.
 *@Development Environment: Eclipse Kepler IDE, Windows 8 Pro 64-bit, JDK7.0_21
 *@Due: 7/29/2013
 */

import java.util.*;
import java.net.*;
import java.io.*;
import java.math.*;

public class NetGraph
{
	private static AdjacencyList G;
	
	//error messages:
	private final static String errInvalidCmdLnArgs="Invalid number of arguments; to view a list of accepted command line parameters, use flag <help>";
	private final static String errInvalidArgs="Invalid system command; to view a list of accepted input commands, type 'H'";
	private final static String errFileNotFound="Unable to find file!";

	public static void main(String[] args)
	{
		if (args.length!=1) perror(errInvalidCmdLnArgs);
		if (args.length==1) //if cmdln arg entered, build an Adjacency List
		{
			if (args[0].equals("help")==true) cmdLnHelp();
			String inputFileName;
			BufferedReader r = null;
			inputFileName = args[0];
			File graphFile = new File(inputFileName);
			try
			{
				G = new AdjacencyList(new BufferedReader(new FileReader(graphFile))); //calls the adjacency list constructor taking a bufferedreader object
			}
			catch (FileNotFoundException e)
			{
				e.printStackTrace();
				perror(errFileNotFound);
			}
		}
		System.out.println("\n\n\n                                     <N E T G R A P H>              \n"); 
		System.out.println("                                  Network Simulation Tool             \n\n\n\n\n\n"); 
		System.out.println("                                 Programmed by Tyler Raborn              "); 
		System.out.println("                                (C) 2013 All Rights Reserved              \n\n\n"); 
		prompt();
	}

	//NetGraph static methods:
	static void prompt()
	{
		System.out.println("Enter a command, or type 'H' to display instructions.");
		Scanner s = new Scanner(System.in);
		String input = s.nextLine();
		if (input.equals("R")==true) report();
		if (input.equals("M")==true) prims();
		//if (input.equals("T")==true) G.testSpace();
		if (input.equals("V")==true) G.view();
		if (input.charAt(0)=='S') shortestPath(getInts(input));
		if (input.charAt(0)=='P') distinctPaths(getInts(input));
		if (input.charAt(0)=='H') help();
		if (input.charAt(0)=='D') down(getInts(input));
		if (input.charAt(0)=='U') up(getInts(input));
		if (input.charAt(0)=='C') change(getInts(input));
		if (input.equals("Q")==true) exit(0);
		prompt();
	}
	static void report() //display the current active network (all active nodes and edges, including edge weights) show the status of the network (connected or not); show the connected components of the network
	{
		System.out.println("\nNetwork Summary:              \n");
		G.report();
		//G.view();
	}
	static void prims()
	{
		System.out.println("\nNetwork Minimum Spanning Tree Summary:              \n");
		G.prims();
	}
	static void shortestPath(int[] arg)
	{
		G.djikstras(arg[0], arg[1]);
	}
	static void distinctPaths(int[] arg)
	{
		//System.out.println(arg[0] + " " + arg[1] + " " + arg[2]);
		G.distinctPaths(arg[0], arg[1], arg[2]);
	}
	static void down(int[] arg)
	{
		
		G.disableNode(arg[0]);
		//G.report();
		//System.out.println("\nNetwork Summary:              \n");
		//G.view();
	}
	static void up(int[] arg)
	{
		G.restoreNode(arg[0]);
		//G.report();
		//System.out.println("\nNetwork Summary:              \n");
		//G.view();
	}
	static void change(int[] arg)
	{
		G.modifyWeight(arg[0], arg[1], arg[2]);
		G.view();
	}

	//TODO: REPLACE ALL USES OF GETTWOINTS() ABOVE WITH THIS FUNCTION INSTEAD.
	static int[] getInts(String s) //reads in a string of integers seperated by spaces, and transforms them into an int array. Used for arguments when calling adjacencylist functions.
	{
		s = s.substring(2, s.length());
		String[] strArray = s.split(" ");
		int[] intArray = new int[strArray.length];
		for (int i = 0; i <= strArray.length-1; i++)
		{
			intArray[i] = Integer.parseInt(strArray[i]);
		}
		return intArray;
	}

	final static void cmdLnHelp()
	{
		System.out.println(
								  "\nValid command line arguments:"
								+ "\njava NetGraph <help> - displays valid cmd line args"
								+ "\njava NetGraph <filename.txt> - accepts a specially formatted .txt file and converts it into an undirected weighted graph."
						  );
		System.exit(0);
	}
	final static void help()
	{
		System.out.println (
								  "\nVALID INTERACTIVE COMMANDS:"   
					            + "\n -R (eport) . . . . . . . . . . : display the current active network (all active nodes and edges, including edge weights); show the status of the network (connected or not); show the connected components of the network\n" 
								+ "\n -M (inimum Spanning Tree). . . : show the vertices and edges (with weights) in the current minimum spanning tree of the network" 
								+ "\n -H (elp) . . . . . . . . . . . : display a list of all valid arguments."
								+ "\n -S <i> <j> . . . . . . . . . . : display the shortest path (by latency) from vertex i to vertex j in the graph" 
								+ "\n -P <i> <j> <x> . . . . . . . . : display each of the distinct paths (no common edges) from vertex i to vertex j with total weight less than or equal to x. All of the vertices and edges in each path should be shown and the total number of distinct paths should be shown at the end.\n" 
								+ "\n -D <i> . . . . . . . . . . . . : node i in the graph will go down. All edges incident upon i will be removed from the graph as well." 
								+ "\n -U <i> . . . . . . . . . . . . : node i in the graph will come back up. All edges previously incident upon i (before it went down) will now come back online with their previous weight values.\n" 
								+ "\n -C <i> <j> <x> . . . . . . . . : change the weight of edge (i, j) (and of edge (j, i)) in the graph to value x. If x is <= 0 the edge(s) should be removed from the graph. If x >= 0 and edge (i, j) previously did not exist, create it.\n" 
								+ "\n -Q (uit) . . . . . . . . . . . : Exit the program.\n"
							);
		prompt();
	}
	final static void exit(int cond) //exit the program
	{
		System.exit(cond);
	}
	final static void perror(final String msg) //error handling
	{
		System.err.println(msg);
		System.exit(-1);
	}




}

