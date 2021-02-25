import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Scanner;

public class DNSLookUp{
    public static void main(String[] args) {
        InetAddress hostAddress;
        Scanner sc = new Scanner(System.in);
        String s = sc.nextLine();
        try{
            hostAddress = InetAddress.getByName(s);
            System.out.println(hostAddress.getHostAddress());
        }catch(UnknownHostException uhe){
            System.err.println("Unknown host : " + args[0]);
        }
    }
}