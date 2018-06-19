package com.imkiva.kivm;

public class ThreadExceptionTest {
    static class MyThread extends Thread {
        public void run() {
            for (int i = 0; i < 10; ++i) {
                System.out.println(getName() + ": " + i);
            }
            throw new RuntimeException("exception thrown from mt thread");
        }
    }

    public static void main(String[] args) throws Exception {
        MyThread t = new MyThread();
        t.start();
        t.join();
    }
}
