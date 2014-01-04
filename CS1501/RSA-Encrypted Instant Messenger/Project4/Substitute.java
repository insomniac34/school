/* @Programmer: Tyler Raborn
 * @Course: CS1501 - Summer 2013
 * @Description: This class implements a substitution cipher.
 * @Class Dependencies: Implements Interface SymCipher
 * @Development Environment: Eclipse Kepler/JDK 7/Windows 8 Pro 64-bit
 */

import java.math.*;
import java.util.*;

public class Substitute implements SymCipher 
{
	//Substitute Fields:
	private final int dim = 256; //array size
	private final int byteInc = 255; //Byte Increment amount necessary to convert negative byte vals when dealing with Java's byte representation scheme
	private byte[] subArray; 
	private byte[] inverseSubArray; //maintains correlation b/w original and encrypted messages
	private int temp; //used to hold the calculated valid array index when dealing with Java's wonky negative byte values
	
	//Substitute Constructors: 
	public Substitute() //The parameterless constructor will create a random dim byte array which is a permutation of the dim possible byte values and will serve as a map from bytes to their substitution values. 
	{		
		ArrayList<Byte> byteList = new ArrayList<Byte>(dim); //temporary ArrayList structure is created in order to make use of util.Collections.shuffle algorithm for randomly permuting (see below)
		subArray = new byte[dim];
		inverseSubArray = new byte[dim];
		for (int i = 0; i <= subArray.length-1; i++) 
		{
			byteList.add((byte)i); //fill ArrayList with all byte vals
		}
		Collections.shuffle(byteList); //randomly permute ArrayList
		for (int ii = 0; ii <= subArray.length-1; ii++) 
		{
			subArray[ii] = (byte)byteList.get(ii); //load randomly permuted Byte() object primitive values into primitive byte array
			if (subArray[ii] < 0) temp = (subArray[ii] + byteInc); //if the encoded message's byte is LESS THAN 0, modify the value so that it's NOT negative and remains a valid index.
			else temp = subArray[ii];
			inverseSubArray[temp] = (byte)ii;
		} //BAM we now have a randomly permuted 256-element array of type byte[] as well as it's inverse for decrypt...
	}
	public Substitute(byte[] byteArray)
	{
		this.subArray = byteArray;
		inverseSubArray = new byte[dim];
		for (int i = 0; i <= inverseSubArray.length-1; i++)
		{
			if (subArray[i] < 0) temp = (subArray[i] + byteInc); //if the encoded message's byte is LESS THAN 0, modify the value so that it's NOT negative and remains a valid index.
			else temp = subArray[i];
			inverseSubArray[temp] = (byte)i;
		}
	}
	
	//Substitute Methods:
	public byte[] getKey()
	{
		return subArray;
	}
	public byte[] encode(String S)
	{
		System.out.println("\nENCRYPTING: ");
		System.out.println("Original String Message: " + S);
		byte[] msg = S.getBytes(); //get byte[] representation of message
		System.out.println("Corresponding Byte Array: " + Arrays.toString(msg));
		byte[] encodedMsg = new byte[msg.length]; //for holding encrypted data
		for (int i = 0; i <= msg.length-1; i++)
		{
			encodedMsg[i] = subArray[msg[i]];
		}
		System.out.println("Encrypted Byte Array: " + Arrays.toString(encodedMsg));
		return encodedMsg;
	}
	public String decode(byte[] encodedMsg)
	{
		System.out.println("\nDECRYPTING: ");
		System.out.println("Array of Encrypted Bytes: " + Arrays.toString(encodedMsg));
		int charVal; //holds individual byte values from the array as they are processed
		byte[] decodedMsg = new byte[encodedMsg.length];
		for (int i = 0; i <= encodedMsg.length-1; i++)
		{
			charVal = encodedMsg[i];
			if (charVal < 0)
				charVal+=byteInc;
			if (inverseSubArray[charVal] < 0)
				charVal = inverseSubArray[charVal] + byteInc;
			else charVal = inverseSubArray[charVal]; //NOW, we can use charVal as the index in inverseSubArray to locate the original value in Msg
			decodedMsg[i] = (byte)charVal;
		}
		System.out.println("Decrypted Array of Bytes: " + Arrays.toString(decodedMsg));
		System.out.println("Corresponding String: " + new String(decodedMsg));
		return new String(decodedMsg);
	}
}
