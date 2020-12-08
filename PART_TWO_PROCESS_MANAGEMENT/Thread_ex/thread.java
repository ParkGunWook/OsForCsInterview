package os_helper;

import java.util.concurrent.*;

class Summation implements Callable<Integer>
{
	private int upper;
	public Summation(int upper) {
		this.upper = upper;
	}
	
	public Integer call() {
		int sum = 0;
		for(int i=1; i<=upper;i++) sum+=i;
		
		return new Integer(sum);
	}
}


public class Driver {
	public static void main(String[] args) {
		int upper = 10;
		
		ExecutorService pool = Executors.newSingleThreadExecutor();
		Future<Integer> result = pool.submit(new Summation(upper));
		
		try {
			System.out.println("sum = " + result.get());
		} catch(InterruptedException | ExecutionException ie) { }
	}
}
