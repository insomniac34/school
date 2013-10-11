/*
 *@Programmer: Tyler Raborn
 *@Assignment: Project 5 - Network Graph Representation
 *@Description: Interface for the essential methods of a graph representation.
 *@Development Environment: Eclipse Kepler, Windows 8 Pro 64-bit, JDK7.0_21
 *@Due: 7/29/2013
 */

public abstract interface UndirectedGraph
{
	abstract void prims(); //uses prim's algorithm to calculate the minimum spanning tree.

	abstract void djikstras(int start, int end); //uses djikstra's algorithm to calculate the shortest path between two vertices.

	abstract void disableNode(int node); //temporarily brings a network node down.

	abstract void restoreNode(int node); //brings the aforementioned network node back up.

	abstract void distinctPaths(int a, int b, int w); //calculates all distinct paths between vertices a and b in the graph.

	abstract void modifyWeight(int v0, int v1, int w); //modifies, deletes or inserts an edge based on the spec sheet's definitions.

}
