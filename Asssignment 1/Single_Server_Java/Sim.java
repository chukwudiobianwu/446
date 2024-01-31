import java.util.*;
import java.util.stream.DoubleStream;
class Sim {

    // Class Sim variables
    public static double Clock, MeanInterArrivalTime, MeanServiceTime, SIGMA, LastEventTime,
           TotalBusy, MaxQueueLength, SumResponseTime;
    public static long  NumberOfCustomers, QueueLength, NumberInService,
           TotalCustomers, NumberOfDepartures, LongService;

    public final static int arrival = 1;
    public final static int departure = 2;

    // Calculate the mean service and interarrival time.
    public static double TotalServiceTime = 0;
    public static double TotalInterArrivalTime = 0;

    public static EventList FutureEventList;
    public static Queue Customers;
    public static Random stream;

    public static void main(String argv[]) {

        MeanInterArrivalTime = 2.857; // Mean, not rate.
        MeanServiceTime      = 2; // Mean, not rate.
        TotalCustomers       = 50000;

        FutureEventList = new EventList();
        Customers = new Queue();

        int numReplications = 10;
        double[] delays = new double[numReplications];

        // Run 10 replications
        for (int replication = 0; replication < numReplications; replication++) {
            // Reset variables for each replication
            Clock = 0.0;
            TotalBusy = 0;
            MaxQueueLength = 0;
            SumResponseTime = 0;
            NumberOfDepartures = 0;
            LongService = 0;
          long seed = Long.parseLong(argv[replication]);
        stream = new Random(seed);
            Initialization();

            // Loop until first "TotalCustomers" have departed
            while (NumberOfDepartures < TotalCustomers) {
                Event evt = (Event) FutureEventList.getMin();
                FutureEventList.dequeue();
                Clock = evt.get_time();
                if (evt.get_type() == arrival) {
                    ProcessArrival(evt);
                } else {
                    ProcessDeparture(evt);
                }
            }

            // Record total delay for the replication
            delays[replication] = SumResponseTime / TotalCustomers;
        }

        // a. Point estimator, confidence interval, and prediction interval
        double meanDelay = DoubleStream.of(delays).average().orElse(0);
        double stdDevDelay = Math.sqrt(Arrays.stream(delays).map(d -> Math.pow(d - meanDelay, 2)).sum() / (numReplications - 1));
        double alpha = 0.05;
        double zValue = 1.96; // For a 95% confidence interval
        double marginError = zValue * stdDevDelay / Math.sqrt(numReplications);
        double lowerCI = meanDelay - marginError;
        double upperCI = meanDelay + marginError;
        double epsilon = 0.5; // Prediction interval width

        // b. Check if simulation data is consistent with system behavior
        double observedDelay = 5.0; // Average delay from system validation
        double lowerPI = meanDelay - epsilon;
        double upperPI = meanDelay + epsilon;

        // c. Repeat with 50000 customers
        TotalCustomers = 50000;
        delays = new double[numReplications];

        // Run 10 replications with 50000 customers
        for (int replication = 0; replication < numReplications; replication++) {
            // Reset variables for each replication
            Clock = 0.0;
            TotalBusy = 0;
            MaxQueueLength = 0;
            SumResponseTime = 0;
            NumberOfDepartures = 0;
            LongService = 0;

            Initialization();

            // Loop until first "TotalCustomers" have departed
            while (NumberOfDepartures < TotalCustomers) {
                Event evt = (Event) FutureEventList.getMin();
                FutureEventList.dequeue();
                Clock = evt.get_time();
                if (evt.get_type() == arrival) {
                    ProcessArrival(evt);
                } else {
                    ProcessDeparture(evt);
                }
            }
            ReportGeneration();
            // Record total delay for the replication
            delays[replication] = SumResponseTime / TotalCustomers;
        }

        // Print results
        System.out.println("a. Point Estimator for Delay: " + meanDelay);
        System.out.println("   Confidence Interval: [" + lowerCI + ", " + upperCI + "]");
        System.out.println("   Prediction Interval: [" + lowerPI + ", " + upperPI + "]");
        System.out.println("b. Is Simulation Data Consistent with System Behavior?");
        System.out.println("   Observed Delay: " + observedDelay);
        System.out.println("   Prediction Interval for Observed Delay: [" + (observedDelay - epsilon) + ", " + (observedDelay + epsilon) + "]");
        System.out.println("c. Repeat with 50000 Customers:");
        System.out.println("   ... (print relevant information)");
    }
    // seed the event list with TotalCustomers arrivals
    public static void Initialization() {
        Clock = 0.0;
        QueueLength = 0;
        NumberInService = 0;
        LastEventTime = 0.0;
        TotalBusy = 0;
        MaxQueueLength = 0;
        SumResponseTime = 0;
        NumberOfDepartures = 0;
        LongService = 0;

        // create first arrival event
        Event evt = new Event(arrival, exponential(stream, MeanInterArrivalTime));
        FutureEventList.enqueue(evt);
    }

    public static void ProcessArrival(Event evt) {
        Customers.enqueue(evt);
        QueueLength++;
        // if the server is idle, fetch the event, do statistics
        // and put into service
        if (NumberInService == 0) {
            ScheduleDeparture();
        } else {
            TotalBusy += (Clock - LastEventTime);  // server is busy
        }

        // adjust max queue length statistics
        if (MaxQueueLength < QueueLength) {
            MaxQueueLength = QueueLength;
        }

        // schedule the next arrival
        double next_interarrival_time = exponential(stream, MeanInterArrivalTime);
        TotalInterArrivalTime += next_interarrival_time;
        Event next_arrival = new Event(arrival, Clock+next_interarrival_time);

        FutureEventList.enqueue(next_arrival);
        LastEventTime = Clock;
    }

    public static void ScheduleDeparture() {
        double ServiceTime;
        // get the job at the head of the queue
        while ((ServiceTime = exponential(stream, MeanServiceTime)) < 0);
        TotalServiceTime += ServiceTime;
        Event depart = new Event(departure,Clock+ServiceTime);
        FutureEventList.enqueue( depart );
        NumberInService = 1;
        QueueLength--;
    }

    public static void ProcessDeparture(Event e) {
        // get the customer description
        Event finished = (Event) Customers.dequeue();
        // if there are customers in the queue then schedule
        // the departure of the next one
        if (QueueLength > 0) {
            ScheduleDeparture();
        } else {
            NumberInService = 0;
        }
        // measure the response time and add to the sum
        double response = (Clock - finished.get_time());
        SumResponseTime += response;
        if (response > 4.0) {
            LongService++; // record long service
        }
        TotalBusy += (Clock - LastEventTime );
        NumberOfDepartures++;
        LastEventTime = Clock;
    }

    public static void ReportGeneration() {
        double RHO   = TotalBusy/Clock;
        double AVGR  = SumResponseTime/TotalCustomers;
        double PC4   = ((double)LongService)/TotalCustomers;


        System.out.println( "SINGLE SERVER QUEUE SIMULATION - SECURITY CHECKIN ");
        System.out.println( "\tMEAN INTERARRIVAL TIME                         " + (TotalInterArrivalTime / TotalCustomers) );
        System.out.println( "\tMEAN SERVICE TIME                              " + (TotalServiceTime / TotalCustomers) );
        System.out.println( "\tNUMBER OF CUSTOMERS SERVED                     " + TotalCustomers );
        System.out.println();
        System.out.println( "\tSERVER UTILIZATION                             " + RHO );
        System.out.println( "\tMAXIMUM LINE LENGTH                            " + MaxQueueLength );
        System.out.println( "\tAVERAGE RESPONSE TIME (w)                      " + AVGR + "  MINUTES" );
        System.out.println( "\tPROPORTION WHO SPEND FOUR ");
        System.out.println( "\t MINUTES OR MORE IN SYSTEM                     " + PC4 );
        System.out.println( "\tSIMULATION RUNLENGTH                           " + Clock + " MINUTES" );
        System.out.println( "\tNUMBER OF DEPARTURES                           " + TotalCustomers );
    }

    public static double exponential(Random rng, double mean) {
        return -mean*Math.log( rng.nextDouble() );
    }

    public static double SaveNormal;
    public static int NumNormals = 0;
    public static final double  PI = 3.1415927 ;

    public static double normal(Random rng, double mean, double sigma) {
        double ReturnNormal;
        // should we generate two normals?
        if (NumNormals == 0) {
            double r1 = rng.nextDouble();
            double r2 = rng.nextDouble();
            ReturnNormal = Math.sqrt(-2*Math.log(r1))*Math.cos(2*PI*r2);
            SaveNormal   = Math.sqrt(-2*Math.log(r1))*Math.sin(2*PI*r2);
            NumNormals = 1;
        } else {
            NumNormals = 0;
            ReturnNormal = SaveNormal;
        }
        return ReturnNormal*sigma + mean ;
    }
}