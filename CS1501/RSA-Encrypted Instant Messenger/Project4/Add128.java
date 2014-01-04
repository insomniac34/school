/* @Programmer: Tyler Raborn
 * @Course: CS1501 - Summer 2013
 * @Description: This class implements the 'Add128' cipher.
 * @Class Dependencies: Implements Interface SymCipher
 * @Development Environment: Eclipse Kepler/JDK 7/Windows 8 Pro 64-bit
 */

import java.util.*;
import java.math.*;

public class Add128 implements SymCipher
{
	//Add128 Fields:
	private final int dim = 128;
	private byte[] additiveKey;
	
	//Add128 Constructors:
	public Add128()
	{
		ArrayList<Byte> byteList = new ArrayList<Byte>(dim);
		additiveKey = new byte[dim];
		for (int i = 0; i <= additiveKey.length-1; i++)
		{
			byteList.add((byte)i);
		}
		Collections.shuffle(byteList); //randomly permute
		for (int ii = 0; ii <= additiveKey.length-1; ii++)
		{
			this.additiveKey[ii] = (byte)byteList.get(ii);
		} 
	}
	public Add128(byte[] byteArray) //The other constructor will use the byte array parameter as its key. The SecureChatClient will call the parameterless constructor and the SecureChatServer calls the version with a parameter.
	{
		this.additiveKey = byteArray;
	}
	
	//Add128 Methods:
	public byte[] getKey()
	{
		return this.additiveKey;
	}
	public byte[] encode(String msg)
	{
		System.out.println("\nENCRYPTING: ");
		System.out.println("Original String Message: " + msg);
		byte[] unsecuredMsg = msg.getBytes(); //convert msg to be encoded into byte array
		byte[] encodedMsg = new byte[unsecuredMsg.length];
		System.out.println("Corresponding Byte Array: " + Arrays.toString(unsecuredMsg));
		for(int i = 0, j = 0; i <= encodedMsg.length-1; i++, j++)
		{
			if (j==encodedMsg.length) 
			{
				break;
			}
			if ((i == this.additiveKey.length-1) && (j != encodedMsg.length-1)) //if the end of the ADDITIVE key is reached and the end of the encodedMsg has NOT been reached...
			{
				i = 0; //reset i...do NOTHING to j, as j represents the actual length traversed...
			}
			encodedMsg[j] = (byte)(unsecuredMsg[j] + additiveKey[i]);
		}
		System.out.println("Encrypted Byte Array: " + Arrays.toString(encodedMsg));
		return encodedMsg;
	}
	public String decode(byte[] encodedMsg)
	{
		System.out.println("\nDECRYPTING: ");
		System.out.println("Array of Encrypted Bytes: " + Arrays.toString(encodedMsg));
		byte[] decodedMsg = new byte[encodedMsg.length];
		for (int i = 0, j = 0; i <= encodedMsg.length-1; i++, j++)
		{
			if (j==encodedMsg.length)
			{
				break;
			} 
			if ((i == this.additiveKey.length-1) && (j != encodedMsg.length-1)) //if the end of the ADDITIVE key is reached and the end of the encodedMsg has NOT been reached...
			{
				i = 0; //reset i...do NOTHING to j, as j represents the actual length traversed...
			}
			encodedMsg[j]-=additiveKey[i];
			decodedMsg[j] = (byte)encodedMsg[j];
		}
		System.out.println("Decrypted Array of Bytes: " + Arrays.toString(decodedMsg));
		System.out.println("Corresponding String: " + new String(decodedMsg));
		return new String(decodedMsg);
	}
}

