import java.io.*;
import java.nio.channels.*;

public class File_lock{
    public static final boolean EXCLUSIVE = false;
    public static final boolean SHARED = true;

    public static void main(String args[]) throws IOException {
        FileLock sharedLock = null;
        FileLock exclusiveLock = null;

        try{
            RandomAccessFile raf = new RandomAccessFile("file.txt", "rw");
            
            //get the channel for the file
            FileChannel ch = raf.getChannel();

            //This locks the first half of the file -exclusive
            exclusiveLock = ch.lock(0, raf.length()/2, EXCLUSIVE);

            exclusiveLock.release();

            //This locks the second half of the file - shared
            sharedLock = ch.lock(raf.length()/2+1, raf.length(), SHARED);

            sharedLock.release();
        }catch(java.io.IOException ioe){
            System.err.println(ioe);
        }finally{
            if(exclusiveLock != null) exclusiveLock.release();
            if(sharedLock != null) sharedLock.release();
        }
    }
}