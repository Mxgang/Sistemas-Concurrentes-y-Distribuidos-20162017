import monitor.*;
import java.util.Random;

class Barberia extends AbstractMonitor {
	
	private int cliente = 0;

	private Condition silla = makeCondition();
	private Condition salaEspera = makeCondition();	
	private Condition barbero = makeCondition();
	
	public void cortarPelo(){	
		enter();
			
			if (!salaEspera.isEmpty()) {				
				cliente++;
				System.out.println("Ha llegado el cliente n."+cliente);
				salaEspera.await();
			} else if (salaEspera.isEmpty() && !silla.isEmpty()) {
				cliente++;
				System.out.println("Ha llegado el cliente n."+cliente+" y ha despertado al barbero");
				salaEspera.await();
			}
			barbero.signal();
			silla.await();
		leave();
	}

	public void siguenteCliente(){
		enter();
			if (salaEspera.isEmpty() && silla.isEmpty()){
				System.out.println("No hay clientes, barbero duerme...");
				barbero.await();
			} 
			else if (silla.isEmpty()) {
				salaEspera.signal();
				cliente--;
				System.out.println("Siguiente! Ahora hay "+cliente+" sillas ocupadas.");
			}
		leave();
	}

	public void finCliente(){
		enter();
			System.out.println("Cliente pelado!");
			silla.signal();
		leave();
	}
}

class aux{
	static Random genAlea = new Random() ;
	static void dormir_max( int milisecsMax ) {
		try {
			Thread.sleep( genAlea.nextInt( milisecsMax ) ) ;
		} catch( InterruptedException e ) {
			System.err.println("sleep interumpido en 'aux.dormir_max()'");
		}
	}
}

class Barbero implements Runnable {
	private int anyVar;
	public Thread thr;		
	private Barberia barberia;		
	public Barbero(String name, int tNumb, Barberia tMon){
		
		anyVar = tNumb;
		barberia = tMon;
		thr = new Thread(this, name);
		System.out.println("La barberia esta abierta ahora!");
	}
	public void run(){
		while(true){
			barberia.siguenteCliente();
			System.out.println ("Pelando al cliente...");
			aux.dormir_max(2500);
			barberia.finCliente();
		}
	}
}

class Cliente implements Runnable {
	private int anyVar;
	public Thread thr;	
	private Barberia barberia;		
	public Cliente(String name, int tNumb, Barberia tMon){
	
		anyVar = tNumb;
		barberia = tMon;
		thr = new Thread(this, name);
	}
	public void run(){
		while(true){
			barberia.cortarPelo();
			aux.dormir_max(2000);
		}
	}
}

class BarberoMonitores{
	public static void main(String[] args){
		try {
			int numberOfThreads = 5, i = 0;
	
			Barberia barberia = new Barberia();
			Cliente[] clients = new Cliente[numberOfThreads];	
			
			for (i=0;i<numberOfThreads;i++){
				clients[i] = new Cliente("Cliente "+(i+1), i+1, barberia);		
			}
			Barbero barbero = new Barbero("Barbero",0,barberia);
			
			barbero.thr.start();
			for (i=0;i<numberOfThreads;i++){
				clients[i].thr.start();		
			}
			for (i=0;i<numberOfThreads;i++){
				clients[i].thr.join();		
			}
			barbero.thr.join();
		} catch (InterruptedException e) { 
			System.out.println ("Ha ocurrido una excepcion");
		}
	}
}

