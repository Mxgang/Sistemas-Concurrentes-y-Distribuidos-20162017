
import monitor.*;

class Estanco extends AbstractMonitor {

    private int estado;
    private Condition[] fumador = new Condition[3];
    private Condition estanquero;

    public Estanco() {
        this.estado = -1;

        for (int i = 0; i < this.fumador.length; i++) {
            this.fumador[i] = makeCondition();
        }

        this.estanquero = makeCondition();
    }

    public void obtenerIngredientes(int miIngrediente) {
        enter();

        if (estado != miIngrediente) {
            this.fumador[miIngrediente].await();
        }

        if (miIngrediente == 0) {
            System.out.println("El primer fumador coge el papel y se va a fumar");
        } else if (miIngrediente == 1) {
            System.out.println("El segundo fumador coge el tabaco y se va a fumar.");
        } else {
            System.out.println("El tercer fumador coge las cerillas y se va a fumar.");
        }

        this.estado = -1;

        this.estanquero.signal();

        leave();
    }

    public void ponerIngredientes(int ingred1) {
        enter();

        if (ingred1 == 0 ) {
            System.out.println("El estanquero pone papel en la mesa.");
            
        } else if (ingred1 == 1) {
            System.out.println("El estanquero pone tabaco en la mesa.");
            
        } else if (ingred1 == 2) {
            System.out.println("El estanquero pone cerillas en la mesa");
            
        }
        this.estado = ingred1;
        this.fumador[this.estado].signal();

        leave();
    }

    public void esperarRecogidaIngredientes() {
        enter();

        if (this.estado != -1) {
            this.estanquero.await();
        }

        System.out.println("La mesa del estanco está vacía.");

        leave();
    }
}

class Fumador implements Runnable {

    private Estanco estanco;
    private int miIngrediente;
    private int tiempo;

    public Fumador(int unIngrediente, Estanco unEstanco) {
        this.miIngrediente = unIngrediente;
        this.estanco = unEstanco;
        this.tiempo = 1000;
    }

    public void run() {
        while (true) {
            this.estanco.obtenerIngredientes(this.miIngrediente);
            try {
                Thread.sleep(tiempo);
            } catch (InterruptedException e) {
            }
            System.out.println(Thread.currentThread().getName() + " termina de fumar");
        }

    }
}

class Estanquero implements Runnable {

    private Estanco estanco;

    public Estanquero(Estanco unEstanco) {
        this.estanco = unEstanco;
    }

    public void run() {
        int ingrediente1;

        while (true) {

            ingrediente1 = (int) (Math.random() * 3.0);


            estanco.ponerIngredientes(ingrediente1);
            estanco.esperarRecogidaIngredientes();
        }

    }
}

public class Fumadores {

    public static void main(String[] args) {
        Estanco estanco = new Estanco();
        Thread hebraEstanquero = new Thread(new Estanquero(estanco));
        Thread[] hebraFumador = new Thread[3];

        hebraFumador[0] = new Thread(new Fumador(0, estanco), "El primer fumador");
        hebraFumador[1] = new Thread(new Fumador(1, estanco), "El segundo fumador");
        hebraFumador[2] = new Thread(new Fumador(2, estanco), "El tercer fumador");

        hebraEstanquero.start();

        for (int i = 0; i < hebraFumador.length; i++) {
            hebraFumador[i].start();
        }
    }
}