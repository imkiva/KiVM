package com.imkiva.kivm;

public class Polymorphism {
    static class People {
        public void call() {
            System.out.println("People");
        }
    }

    static class Boy extends People {
        public void call() {
            System.out.println("Boy");
        }
    }

    static class Girl extends People {
        public void call() {
            System.out.println("Girl");
        }
    }

    public static void call(People p) {
        p.call();
    }

    public static void main(String[] args) {
        People boy = new Boy();
        People girl = new Girl();
        call(boy);
        call(girl);
    }
}
