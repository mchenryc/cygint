import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class TestShutdownHooks {

    private static int tick = 0;

    public static void main(String[] args) throws Exception {
        Runtime.getRuntime().addShutdownHook(new MyHook());

	int period = 1;
	int duration = 20;

	ScheduledExecutorService executor = Executors.newScheduledThreadPool(1);

	Runnable task = () -> System.out.println(String.format("%d/%d", tick++, duration));
	executor.scheduleAtFixedRate(task, 0, period, TimeUnit.SECONDS);

	TimeUnit.SECONDS.sleep(duration);
	executor.shutdownNow();
    }
    
    private static class MyHook extends Thread {
        public void run() {
            System.out.println("Hook called.");
        }
    }
}
