package com.imkiva.kivm;

import java.util.ArrayList;
import java.util.List;

public class ThreadTest {
    private int nextGood = 0;
    private List<Integer> goods = new ArrayList<>();

    class Producer extends Thread {
        int id;

        public Producer(int id) {
            this.id = id;
        }

        @Override
        public void run() {
            while (true) {
                synchronized (Main.class) {
                    while (goods.size() == 10) {
                        try {
                            Main.class.wait();
                        } catch (Exception ignore) {
                        }
                    }

                    int good = nextGood++;
                    goods.add(good);
                    System.out.println("Producer #" + id + ": produced " + good);
                    Main.class.notifyAll();
                }

                try {
                    Thread.sleep(1000);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    class Consumer extends Thread {
        int id;

        public Consumer(int id) {
            this.id = id;
        }

        @Override
        public void run() {
            while (true) {
                synchronized (Main.class) {
                    while (goods.size() == 0) {
                        try {
                            Main.class.wait();
                        } catch (Exception ignore) {
                        }
                    }
                    int good = goods.get(0);
                    goods.remove(0);
                    System.out.println("Consumer #" + id + " consumed " + good);
                    Main.class.notifyAll();
                }
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public static void main(String[] args) {
        new ThreadTest().test();
    }

    private void test() {
        for (int i = 0; i < 3; i++) {
            new Producer(i).start();
        }
        for (int i = 0; i < 10; i++) {
            new Consumer(i).start();
        }
    }
}

