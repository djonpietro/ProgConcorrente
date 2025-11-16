class Primo implements Runnable {
   int num;
   
   public Primo(int n) { 
      num = n; 
   }

   public void run() {
      if (num < 2) {
         System.out.println(num + " não é primo");
         return;
      }
      
      for (int i = 2; i <= Math.sqrt(num); i++) {
         if (num % i == 0) {
            System.out.println(num + " não é primo");
            return;
         }
      }
      System.out.println(num + " é primo");
   }
}