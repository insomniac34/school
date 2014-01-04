// CS 1501 Summer 2013
// Interface for symmetric ciphers to be used with Assignment 4.  This must be
// implemented by both your Add128 and Substitute classes.

public interface SymCipher
{
	// Return an array of bytes that represent the key for the cipher
	public byte [] getKey();	
	
	// Encode the string using the key and return the result as an array of
	// bytes.  Note that you will need to convert the String to an array of bytes
	// prior to encrypting it.
	public byte [] encode(String S);
	
	// Decrypt the array of bytes and generate and return the corresponding String
	public String decode(byte [] bytes);
	
}