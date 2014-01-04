/* @Programmer: Tyler Raborn
 * @Course: CS1501 - Summer 2013
 * @Description: This program implements an RSA-encrypted instant messenger client
 * @Development Environment: Eclipse Kepler/JDK 7/Windows 8 Pro 64-bit
 */

import java.util.*;
import java.io.*;
import java.net.*;
import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.math.*;

public class SecureChatClient extends JFrame implements Runnable, ActionListener 
{
    public static final int PORT = 8765;
    JTextArea outputArea;
    JLabel prompt;
    JTextField inputField;
    String myName, serverName;
    Socket connection;

    ObjectOutputStream curWriter;
    ObjectInputStream curReader;
    BigInteger E; //public key
    BigInteger N; //mod value N
    String cipherName;
    SymCipher cipher;
    BigInteger key;
    BigInteger secureKey;
    private byte[] secureName;
    private byte[] secureMsg;
    private byte[] secureCloseMsg;

    public SecureChatClient()
    {
        try 
        {
            //i/o handling via jPane...
            System.out.println("Starting client...");
            myName = JOptionPane.showInputDialog(this, "Enter your user name: ");
            serverName = JOptionPane.showInputDialog(this, "Enter the server name: ");
            InetAddress addr = InetAddress.getByName(serverName);
			
            //Client connects to server as per specsheet...
            connection = new Socket(addr, PORT); //It opens a connection to the server via a Socket at the server's IP address and port.
            curWriter = new ObjectOutputStream(connection.getOutputStream()); //It creates an ObjectOutputStream on the socket (for writing) 
            curWriter.flush(); //and immediately calls the flush() method (this technicality prevents deadlock)
            curReader = new ObjectInputStream(connection.getInputStream()); //It creates on ObjectInputStream on the socket (be sure you create this AFTER creating the ObjectOutputStream)
            E = (BigInteger)curReader.readObject(); //It receives the server's public key, E, as a BigInteger object
            N = (BigInteger)curReader.readObject(); //It receives the server's public mod value, N, as a BigInteger object
            System.out.println("\nE key Received: " + E);
            System.out.println("\nN key Received: " + N);
            cipherName = (String)curReader.readObject(); //It receives the server's preferred symmetric cipher (either "Sub" or "Add"), as a String object
            if(cipherName.equals("Sub")==true) //Based on the value of the cipher preference, it creates either a Substitute object
            {
                cipher = new Substitute(); //storing the resulting object in a SymCipher variable.
                System.out.println("\nSymmetric Encryption Scheme: Subsitute()");
            }
            else //or an Add128 object
            {
                cipher = new Add128(); //storing the resulting object in a SymCipher variable.
                System.out.println("\nSymmetric Encryption Scheme: Add128()");
            }
            key = new BigInteger(1, cipher.getKey()); //It gets the key from its cipher object using the getKey() method, and then converts the result into a BigInteger object. The 1 ensures a positive #.
            secureKey = key.modPow(E, N);//It RSA-encrypts the BigInteger version of the key using E and N, 
            curWriter.writeObject(secureKey); //and sends the resulting BigInteger to the server (so the server can also determine the key – the server already knows which cipher will be used)
            curWriter.flush();
            secureName = cipher.encode(myName); //It prompts the user for his/her name, then encrypts it using the cipher and sends it to the server. The encryption will be done using the encode() method of the SymCipher interface, and the resulting array of bytes will be sent to the server as a single object using the ObjectOutputStream.
            curWriter.writeObject(secureName);   // Send name to Server.  Server will need this to announce sign-on and sign-off of clients
            curWriter.flush();
            System.out.println("\n<<HANDSHAKE ESTABLISHED>>\n");
            
            this.setTitle(myName);      // Set title to identify chatter

            Box b = Box.createHorizontalBox();  // Set up graphical environment for
            outputArea = new JTextArea(8, 30);  // user
            outputArea.setEditable(false);
            b.add(new JScrollPane(outputArea));

            outputArea.append("Welcome to the Chat Group, " + myName + "\n");

            inputField = new JTextField("");  // This is where user will type input
            inputField.addActionListener(this);

            prompt = new JLabel("Type your messages below:");
            Container c = getContentPane();

            c.add(b, BorderLayout.NORTH);
            c.add(prompt, BorderLayout.CENTER);
            c.add(inputField, BorderLayout.SOUTH);

            Thread outputThread = new Thread(this);  // Thread is to receive strings from Server
            outputThread.start();                    

            addWindowListener(
                new WindowAdapter()
                {
                    public void windowClosing(WindowEvent e) //encrypt and transmit close message
                    { 
                        secureCloseMsg = cipher.encode("CLIENT CLOSING");
                        try 
                        {
                            System.out.println("TRANSMITTING CLOSE SIGNAL");
                            curWriter.writeObject(secureCloseMsg);
                            curWriter.flush();
                        } 
                        catch (IOException e1) 
                        {
                            e1.printStackTrace();
                        }
                        System.exit(0);
                    }
                }
             );

             setSize(500, 200);
             setVisible(true);
        }
    catch (Exception e)
    {
     System.out.println("Problem starting client!");
    }
    }

    public void run()
    {
    while (true)
    {
      try 
      {         
          byte[] encryptedMsg = (byte[])curReader.readObject();
          encryptedMsg = (cipher.decode(encryptedMsg)).getBytes();
          String msg = new String(encryptedMsg);
          outputArea.append(msg+"\n");
      }
      catch (Exception e)
      {
         System.out.println(e +  ", closing client!");
         break;
      }
    }
    System.exit(0);
    }

    public void actionPerformed(ActionEvent e)
    {
        String userName = myName + ": ";
        String currMsg = userName + e.getActionCommand(); // Get input value...
        secureMsg = cipher.encode(currMsg); // encode input value...
        inputField.setText("");
        try 
        {
            curWriter.writeObject(secureMsg);
            curWriter.flush();
        } 
        catch (IOException e1) 
        {
            System.out.println("Writing failed!");
        }   // Add name and send it to Server
    }                                               

    public static void main(String [] args)
    {
    SecureChatClient JR = new SecureChatClient();
    JR.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
    }
}





