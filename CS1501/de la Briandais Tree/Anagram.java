/*
 *@Programmer: Tyler Raborn
 *@email: <tpr11@pitt.edu>
 *@Class: CS-1501 Summer 2013
 *@Assignment: Project 1 - Anagram Finder
 *@Environment: Eclipse Juno IDE, Windows 8 Pro 64-bit, JDK7.0_21
 *@Due: 6/5/2013
 */

//Eclipse Stuff - DELETE BEFORE RUNNING NATIVELY:////////////////////
//package assignment1v1;

//library imports
import java.util.*;
import java.io.*;
import java.math.*;
import java.awt.*;
//import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

//@SuppressWarnings("unused")
public class Anagram {
	public static DictInterface d; //data structure interface; morphs into a DLB or MyDictionary based on user input
	protected final static ArrayList<String> anagramList = new ArrayList<String>(0); //data structure for holding anagrams as they are found
	private static String struct; //input string object; declared outside of main so that text output during testing could display what the user entered.
	public static void main(String[] args) throws FileNotFoundException //input: strings in a text file broken up by lines, can contain spaces. IGNORE SPACES. output: all possible valid words/phrases
	{
		//user I/O handling:
		final String invalidArgs="Invalid number of arguments; accepted input: <dlb or orig> <inputfilename.txt> <outputfilename.txt>";
		final String invalidDS="Invalid data structure entered. accepted input: <dlb or orig> <inputfilename.txt> <outputfilename.txt>";
		if (args.length!=3)
			perror(invalidArgs);
		String dictionaryFileName, inputFileName, outputFileName;
		int sFlag=0;
		struct=args[2];
		if((struct.equals("dlb")!=true) && (struct.equals("orig")!=true))
			perror(invalidDS);
		if(struct.equals("dlb")==true)
		{
			struct="de la Briandais tree";
			sFlag=1;
		}
		else 			
			struct="Sorted Array (Java.util.Collections.ArrayList<E>())";
		dictionaryFileName="dictionary.txt";
		inputFileName=args[0];
		outputFileName=args[1];

		//File I/O objects/tools
		BufferedReader r = null;
		BufferedReader dictReader = null;
		String input = null;
		String dictEntry = null;
		File inFile = new File(inputFileName); //create input file abstraction
		File dictFile = new File(dictionaryFileName); //create dictionary file abstraction
		if (sFlag==1)
			d = new DLB(); //instantiate a DLB object
		else
			d = new MyDictionary(); //instantiate a MyDictionary object
		ArrayList<String> inputList = new ArrayList<String>(); //uses ArrayList<> data structure to hold the strings of text separated by lines as they are read from file
		String sb;
		try //read in input file via a BufferedReader object
		{
			r = new BufferedReader(new FileReader(inFile));
			while ((input = r.readLine())!=null) //build arraylist from words in input file
			{
				sb = new String(removeSpaces(input)); //call to remove spaces 
				inputList.add(sb);
			}
		}
		catch(FileNotFoundException e)
		{
			e.printStackTrace();
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		finally /*when done reading from file, attempt to close file*/ 
		{
			try
			{
				if (r!=null)
				{
					r.close();
				}
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
		}//end try/catch block
		try //read in dictionary contents via a BufferedReader object
		{
			dictReader = new BufferedReader(new FileReader(dictFile));
			while ((dictEntry = dictReader.readLine())!=null) //build dictionary from words in source file
			{
				d.add(removeSpaces(dictEntry)); 
			}
		}
		catch(FileNotFoundException e)
		{
			e.printStackTrace();
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		finally //when done reading from file, attempt to close file. Finally blocks execute no matter what after the Try block finished.
		{
			try
			{
				if (dictReader != null)
				{
					dictReader.close();
				}
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
		} //end file I/O
		DateFormat startDateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss"); //times the start of algorithm. printed at beginning & end of algorithm runtime
		Date startDate = new Date();
		System.out.println("\nAlgorithm beginning run at " + startDateFormat.format(startDate) + "\n");
		for(int inputList_iterator = 0; inputList_iterator <= inputList.size()-1; inputList_iterator++) //for each word of input, call the anagramize() function...
		{
			anagramize(new StringBuilder(), inputList.get(inputList_iterator).toCharArray(), 0, inputList.get(inputList_iterator).length()-1);
		}
		Collections.sort(anagramList); //sort the arrayList alphabetically
		PrintWriter outWriter = new PrintWriter(outputFileName); //puts results into output file...
		for (int i = 0; i <= anagramList.size()-1; i++) //for each anagram, write it to the output file.
		{
			System.out.println("Added " + anagramList.get(i) + " to " + outputFileName);
			outWriter.println(anagramList.get(i)); //print it to the output file
		}
		outWriter.close();
		System.out.println("\nRUN SUMMARY: ");
		System.out.println("Data Structure Used: " + struct);
		System.out.println("Input File Used: " + inputFileName);
		//System.out.println("Output file written to: " + outputFileName);
		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		Date date = new Date();
		System.out.println("Algorithm start time: " + startDateFormat.format(startDate) + "\n" + "Algorithm end time: " + dateFormat.format(date));
	} //end main()

	static void anagramize(StringBuilder s, char[] letters, int start, int l) //replaces elements in letters[] with the char '&', a sentinel of sorts...
	{
		for(int i = 0; i <= letters.length-1; i++) //each letter in letters
		{
			if (letters[i] != '&') //the char & signifies the char has been removed and to not use it...
			{
				char c = letters[i]; 
				s.append(c);
				letters[i] = '&';
				//System.out.println("Now searching " + struct + " for substring " + s.substring(start, s.length()-1) + " in string " + s);
				int ret = d.searchPrefix(s, start, s.length()-1); //see if appending that letter to S's current value creates a word or a prefix
				if (ret == 1 || ret == 3) //if s's current value is a prefix or is a word and a prefix, recursively call this function on s's current value
				{
					anagramize(s, letters, start, l); //recursive call to anagramize on s's current value
				}
				if ((ret == 2) || (ret == 3)) //if s's current value is a word or is a word and a prefix
				{	
					if (s.length()-1 < l) //if the matched word is less than the length of the source word's length l...
					{
						anagramize(new StringBuilder(s).insert(s.length(), " "), letters, s.length()+1, l+1); //recursively call anagramize, but with new lower bounds. The <start> field is now the length of input StringBuilder s, and l remains the same.
					}
					if ((anagramList.contains(s.toString()) != true) && (s.length()-1 == l)) //if the word hasn't already been added and is the appropriate length...
					{
						String dualAnagram = new String(s);
						if ((start!=0) && (anagramList.contains(dualAnagram) != true)) //if the start location does NOT equal the beginning of s
						{
							//System.out.println("Adding " + dualAnagram + " to anagramList");
							anagramList.add(dualAnagram); //add the position-modified string to the list of valid anagrams...
						}
						else if (start==0)
						{
							//System.out.println("Adding " + s + " to anagramList");
							anagramList.add(new String(s)); //stores a newly instantiated copy of s in the list of valid anagrams...
						}
					}
					if (letters.length-1 == 0)
					{
						break; //we are done here...
					}
				}
				letters[i] = c; //reinsert c back into it's original position in letters...
				s.deleteCharAt(s.length()-1); //remove c from END of word
			}
		}
	}
	private static String removeSpaces(String s) //function for removal of spaces in strings
	{
		return s.replaceAll(" ", "");
	}
	private final static void perror(final String msg) //error handling
	{
		System.err.println("ERROR: " + msg);
		System.exit(-1);
	}
}