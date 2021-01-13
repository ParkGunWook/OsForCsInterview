import java.util.concurrent.*;
public class ThreadPoolExample
{
    public static void main(String[] args) {
        int numTasks = Integer.parseInt(args[0].trim());

        ExecutorService pool = Executor.newCachedThreadPool();

        for(int i=0; i< numTasks;i++){
            pool.execute(new Task());
        }
        pool.shutdown();
    }
}