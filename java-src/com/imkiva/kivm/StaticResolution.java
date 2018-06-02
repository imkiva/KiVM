package com.imkiva.kivm;

public class StaticResolution {
    static class People {
    }

    static class Boy extends People {
    }

    static class Girl extends People {
    }

    public static void call(People p) {
        System.out.println("People");
    }

    public static void call(Boy p) {
        System.out.println("Boy");
    }

    public static void call(Girl p) {
        System.out.println("Girl");
    }

    public static void main(String[] args) {
        People boy = new Boy();
        People girl = new Girl();
        call(boy);
        call(girl);
    }
}
