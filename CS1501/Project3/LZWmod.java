
/*@ Programmer: Tyler Raborn
 *@ Class: CS1501 Summer 2013
 *@ Assignment: Project 3 - LZW Compression Algorithm 
 *@ Test Environment: Windows 8 64-bit, Mac OSX Mountain Lion, Ubuntu Linux 12.04
 *
 */
import java.lang.*;
import java.util.*;

public class LZWmod
{
    private static final int R = 256;               // number of pre-loaded ASCII values
    private static final int MAX_CHARS = 65536;     // size of st array for decompression
    private static int L;                           // maximum number of codewords = 2^W
    private static int W;                           // codeword width
    private static String codeWordHandler;           // holds the user's decision on how to handle codeword overflow; n = "do nothing" r = "reset" m = "monitor"
    private static int dictionaryFlag;              // if n, 0 if r, 1, if m, 2

    public static void main(String[] args) 
    {
        int compressFlag=0;
        try
        {
            if (args[1].equals("n") || args[1].equals("r") || args[1].equals("m")) 
            {
                codeWordHandler = args[1];
                compressFlag=1;
            }
        }
        catch (ArrayIndexOutOfBoundsException a) {};
        if      (args[0].equals("-") && compressFlag==1) compress();
        else if (args[0].equals("+")) expand();
        else throw new RuntimeException("Illegal command line argument");
    }

    public static void compress() 
    { 
        W = 9;
        L = 512;

        String input = BinaryStdIn.readString();
        TST<Integer> st = new TST<Integer>();
        int i;
        for (i = 0; i < R; i++) //adds ascii values to trie
            st.put("" + (char) i, i);
        int code = R+1;  // R is codeword for EOF
        i+=1;

        if (codeWordHandler.equals("n")) //do nothing mode
        {
            BinaryStdOut.write(0, 2);
            while (input.length() > 0) 
            {
                //System.err.println("Bitsize: " + W + ", length of String input: " + input.length() + ", Number of words added to dictionary: " + i + ", Total possible words based on cur bitsize: " + L );
                if((i >= L) && (W != 16)) //if number of codewords reaches bitwidth-enforced maximum, increment W...
                {
                    W++;
                    L = (int)Math.pow(2.0, (double) W);
                }
                if (input.length() <= 0)
                {
                    break;  
                }
                String s = st.longestPrefixOf(input);  // Find max prefix match s.
                BinaryStdOut.write(st.get(s), W);      // Print s's encoding.
                int t = s.length();                    // t is the index used to extract the extra char to form s...
                if (t < input.length() && code < L)    // as long as T doesn't go out of bounds for the input string, and as long as the EOF code is less than L, add s to symbol table.
                {
                    st.put(input.substring(0, t + 1), code++); // insert NEW string into TST. ALSO, int code dynamically grows to match # of strings used...
                    i++;
                }
                input = input.substring(t);            // Scan past s in input.
            }
        }
        else if (codeWordHandler.equals("r")) //reset mode
        {
            System.err.println("Now entering Reset mode...");
            BinaryStdOut.write(1, 2);
            while (input.length() > 0) 
            {
                //System.err.println("Bitsize: " + W + ", length of String input: " + input.length() + ", Number of words added to dictionary: " + i + ", Total possible words based on cur bitsize: " + L );
                if((i >= L) && (W != 16)) //if number of codewords reaches bitwidth-enforced maximum, increment W
                {
                    W++;
                    L = (int)Math.pow(2.0, (double) W);
                }
                if (i == MAX_CHARS) // if (number of words added to dictionary is greater than/equal to the bitwidth-enforced max) dictionary.reset()
                {
                    st = new TST<Integer>(); //reset TST
                    for (i = 0; i < R; i++) //re-adds ascii values to TST
                        st.put("" + (char) i, i);

                    //reset L and W:    
                    W = 9;
                    L = (int)Math.pow(2.0, (double)W);
                    code = R+1;  // R is codeword for EOF
                    i+=1;
                }
                if (input.length() <= 0)
                {
                    break;  
                }
                String s = st.longestPrefixOf(input);  // Find max prefix match s...
                BinaryStdOut.write(st.get(s), W);      // Print s's encoding.
                int t = s.length();                    // t is the index used to extract the extra char to form s...
                if (t < input.length() && code < L)    // as long as T doesn't go out of bounds for the input string, and as long as the EOF code is less than L, add s to symbol table.
                {
                    st.put(input.substring(0, t + 1), code++); // insert NEW string into TST. ALSO, int code dynamically grows to match # of strings used...
                    i++;
                }
                input = input.substring(t);            // Scan past s in input.
            }
        }
        else //if (codeWordHandler.equals("m")) //monitoring mode
        {
            System.err.println("Monitoring mode selected.");
            //int ratioFlag=0;
            int monitorFlag=0; //flag only activates once monitoring begins
            int readBits=0;
            int compressedBits=0;
            double compressionRatio=0.0; //ratio of ratio
            //double newRatio;
            double oldRatio=0.0; //used for monitoring ratio of ratio...
            BinaryStdOut.write(2, 2);
            while (input.length() > 0) 
            {
                //System.err.println("Bitsize: " + W + ", length of String input: " + input.length() + ", Number of words added to dictionary: " + i + ", Total possible words based on cur bitsize: " + L );
                if((i >= L) && (W != 16)) //if number of codewords reaches bitwidth-enforced maximum, increment W...
                {
                    W++;
                    L = (int)Math.pow(2.0, (double) W);
                }
                if (compressedBits!=0)
                {
                    compressionRatio = ((double)readBits / (double)compressedBits);    
                }
                if (i == MAX_CHARS || monitorFlag == 1) //begin monitoring process...
                {
                    if (monitorFlag==1) //contains monitoring variables
                    {
                        //System.err.println("Checking current compression ratio: " + (oldRatio / compressionRatio));
                        if ((oldRatio / compressionRatio) > 1.1)
                        {
                            System.err.println("Compression Ratio has exceeded 1.1. Resetting dictionary and all relevant flags...");
                            Scanner scan = new Scanner(System.in);
                            String tempString = scan.nextLine();
                            //reset dictionary:
                            st = new TST<Integer>(); //reset TST
                            for (i = 0; i < R; i++) //re-adds ascii values to TST
                                st.put("" + (char) i, i);
                            //reset L and W:    
                            W = 9;
                            L = (int)Math.pow(2.0, (double)W);
                            code = R+1;  // R is codeword for EOF
                            i+=1;
                            monitorFlag=0;
                            readBits=0;
                            compressedBits=0;                    
                        }
                    }
                    else
                    {
                        System.err.println("MAX_CHARS has been reached. Now beginning monitoring...");
                        oldRatio = compressionRatio;
                        monitorFlag=1;    
                    }
                }
                if (input.length() <= 0)
                {
                    break;  
                }
                String s = st.longestPrefixOf(input);  // Find max prefix match s.
                //System.err.println("s = " + s);
                readBits+=(8 * s.length());            // compound the total number of bits in readBits
                BinaryStdOut.write(st.get(s), W);      // Print s's encoding.
                compressedBits+=W;
                int t = s.length();                    // t is the index used to extract the extra char to form s...
                if (t < input.length() && code < L)    // as long as T doesn't go out of bounds for the input string, and as long as the EOF code is less than L, add s to symbol table.
                {
                    st.put(input.substring(0, t + 1), code++); // insert NEW string into TST. ALSO, int code dynamically grows to match # of strings used...
                    i++;
                }

                input = input.substring(t);            // Scan past s in input.
            }
        }
        //System.err.println("Total number of words written: " + i);
        BinaryStdOut.write(R, W); 
        //BinaryStdOut.write(dictionaryFlag, W); //encode the value of dictionaryFlag in a W-bit number at the VERY end of the file.
        BinaryStdOut.close();
    } 

    public static void expand() 
    {
        W = 9;
        L = 512;
        dictionaryFlag = BinaryStdIn.readInt(2); //read in dictionary handling descriptor...
        String[] st = new String[MAX_CHARS];
        int i; // next available codeword value

        // initialize symbol table with all 1-character strings:
        for (i = 0; i < R; i++)
            st[i] = "" + (char) i;
        st[i++] = "";                        // (unused) lookahead for EOF
        int codeword = BinaryStdIn.readInt(W);
        String val = st[codeword];
        if (dictionaryFlag==0) //do nothing mode
        {
            System.err.println("Dictionary Handling Method: Do Nothing");
            while (true) 
            {
                if((i >= L-1) && (W != 16))
                {
                    W++;
                    L = (int)Math.pow(2.0, (double)W);
                }
                BinaryStdOut.write(val); //write current string val
                codeword = BinaryStdIn.readInt(W); //read a codeword x from input
                if (codeword == R) break;
                String s = st[codeword]; //set s to the value associated with x in symbol table
                //System.err.println(", Lookahead character s: " + s);
                if (i == codeword) s = val + val.charAt(0);   // special case hack
                if (i < L) st[i++] = val + s.charAt(0);
                val = s;
            }
        }
        else if (dictionaryFlag==1) //reset mode
        {
            System.err.println("Dictionary Handling Method: Reset");
            while (true) 
            {
                if (i == (MAX_CHARS-1))
                {
                    st = new String[MAX_CHARS];
                    for (i = 0; i < R; i++) //re-initialize symbol table with all 1-character strings
                        st[i] = "" + (char) i;
                    W = 9;
                    L = (int)Math.pow(2.0, (double)W);
                    st[i++] = "";                        // (unused) lookahead for EOF
                    codeword = BinaryStdIn.readInt(W);
                    val = st[codeword];
                    //System.err.println("DICTIONARY HAS BEEN RESET: codeword = " + codeword + "val = " + val);
                }
                if((i >= L-1) && (W != 16))
                {
                    W++;
                    L = (int)Math.pow(2.0, (double)W);
                }
                BinaryStdOut.write(val); //write current string val
                codeword = BinaryStdIn.readInt(W); //read a codeword x from input
                if (codeword == R) break;
                String s = st[codeword]; //set s to the value associated with x in symbol table 
                if (i == codeword) s = val + val.charAt(0);   // special case hack
                if (i < L) st[i++] = val + s.charAt(0);
                val = s;
                //System.err.println("i: " + i + ", Lookahead character s: " + s + ", val: " + val);
            }
        }
        else //if (dictionaryFlag==2) //monitor mode
        {
            int monitorFlag=0; //flag only activates once monitoring begins
            int readBits=0; //counts # of bits read in
            int compressedBits=0; //counts # of bits being compressed
            double compressionRatio=0.0;
            //double newRatio;
            double oldRatio=0.0; //used for monitoring ratio of ratio...
            //System.err.println("Dictionary Handling Method: Compression Ratio Monitoring");
            while (true) 
            {
                //System.err.println("current i value: " + i);
                if (compressedBits!=0)
                {
                    compressionRatio = ((double)readBits / (double)compressedBits);    
                }

                if((i >= L-1) && (W != 16))
                {
                    W++;
                    L = (int)Math.pow(2.0, (double)W);
                }
                BinaryStdOut.write(val); //write current string val
                //System.err.println("val = " + val);
                if (i == MAX_CHARS-1 || monitorFlag == 1) //begin monitoring process...
                {
                    if (monitorFlag==1) //contains monitoring variables
                    {
                        //System.err.println("Checking current compression ratio: " + (oldRatio / compressionRatio);
                        if ((oldRatio / compressionRatio) > 1.1)
                        {
                            System.err.println("Compression Ratio has exceeded 1.1. Resetting dictionary and all relevant flags...");
                            //reset dictionary:
                            st = new String[MAX_CHARS];
                            for (i = 0; i < R; i++) //re-initialize symbol table with all 1-character strings
                                st[i] = "" + (char) i;
                            W = 9;
                            L = (int)Math.pow(2.0, (double)W);
                            st[i++] = "";                        // (unused) lookahead for EOF
                            codeword = BinaryStdIn.readInt(W);
                            val = st[codeword];
                            monitorFlag=0;
                            readBits=0;
                            compressedBits=0;                    
                        }
                    }
                    else
                    {
                        //System.err.println("MAX_CHARS has been reached. Now beginning monitoring...");
                        oldRatio = compressionRatio;
                        monitorFlag=1;    
                    }
                }
                codeword = BinaryStdIn.readInt(W); //read a codeword x from input
                compressedBits+=W;
                //System.err.println("codeword: " + codeword);
                if (codeword == R) break;
                String s = st[codeword]; //set s to the value associated with x in symbol table
                //System.err.println("About to use codeword " + codeword + " to access array value " + st[codeword]);
                //System.err.println(", Lookahead character s: " + s);
                if (i == codeword) s = val + val.charAt(0);   // special case hack
                if (i < L) st[i++] = val + s.charAt(0);
                val = s;
                readBits+=(8 * val.length());
            }
        }
        BinaryStdOut.close();
    }
}
