package com.imkiva.kivm;

public class PackagePrivateTest {
    static void packagePrivateMethod() {
        System.out.println("package private");
    }

    static class Sub extends PackagePrivateTest {
        static void f() {
            packagePrivateMethod();
        }
    }

    public static void main(String[] args) {
        Sub.f();
    }
}
