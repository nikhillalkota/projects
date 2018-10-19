/** Nikhil Lalkota(nl269),Vaishnavi Anjanegowda(va273),Jacqueline Andrade(jpa34)
 **/
import java.io.*;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.*;
public class SimpleCracker
{

    public static String toHex(byte[] bytes) {
        BigInteger bi = new BigInteger(1, bytes);
        return String.format("%0" + (bytes.length << 1) + "X", bi);
    }

    public static String stringToMd5(String input) throws Exception {
        //Setup a MessageDigest for MD5
        MessageDigest mdc = MessageDigest.getInstance("MD5");
        mdc.reset();

        //Setup the MessageDigest with our input string
        mdc.update(input.getBytes());

        //Convert the string's digest to HEX
        String hash1 = toHex(mdc.digest());
        return hash1;
    }


    public static void main(String[] args) throws Exception
    {
        //reading password file
        FileReader pr =
                new FileReader("/root/Desktop/cs645/common-passwords.txt");
        BufferedReader buffPass = new BufferedReader(pr);
        String pa;
        ArrayList<String> password = new ArrayList<>();
        while((pa = buffPass.readLine())!= null){
            //System.out.println(pa);
            password.add(pa);
        }
        //reading shadow file
        FileReader fr =
                new FileReader("/root/Desktop/cs645/shadow-simple.txt");
        BufferedReader buff = new BufferedReader(fr);
        String st;
        //String[][] sa = new String[10][3];
        //int count = 0;
        while((st = buff.readLine()) != null) {
            String sa[] = st.split(":");
            for(String x:password)
            {
                String y=sa[1].concat(x);
                //System.out.println(y);
                y =stringToMd5(y);
                //count++;
                //System.out.println(y);
                if(y.equals(sa[2])) {
                    System.out.println("username: " +sa[0]+ " password: " +x);
                }
            }
        }

        fr.close();


    }
}
