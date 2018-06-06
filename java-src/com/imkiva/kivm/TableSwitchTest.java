package com.imkiva.kivm;

import sun.misc.Unsafe;

public class TableSwitchTest {
    public static void main(String[] args) {
        Unsafe unsafe = Unsafe.getUnsafe();
        long a = unsafe.allocateMemory(8);

        try {
            unsafe.putLong(a, 0x0102030405060708L);
            byte b = unsafe.getByte(a);
            switch (b) {
                case 1:
                    System.out.println("1");
                    break;
                case 2:
                    System.out.println("2");
                    break;
                case 3:
                    System.out.println("3");
                    break;
                case 4:
                    System.out.println("4");
                    break;
                case 5:
                    System.out.println("5");
                    break;
                case 6:
                    System.out.println("6");
                    break;
                case 7:
                    System.out.println("7");
                    break;
                case 8:
                    System.out.println("8");
                    break;
                default:
                    System.out.println("default");
                    break;
            }
        } finally {
            unsafe.freeMemory(a);
        }
    }
}
