package com.imkiva.kivm;

public class LambdaTest {
    public static void f(Runnable r) {
        r.run();
    }

    public static void main(String... args) {
        f(() -> System.out.println("Hello Lambda"));
    }
}