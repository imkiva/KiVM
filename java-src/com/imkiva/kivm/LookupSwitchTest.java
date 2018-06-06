package com.imkiva.kivm;

import sun.misc.Unsafe;

public class LookupSwitchTest {
    public static void main(String[] args) {
        Unsafe unsafe = Unsafe.getUnsafe();
        long a = unsafe.allocateMemory(8);
        try {
            unsafe.putLong(a, 0x0102030405060708L);
            byte b = unsafe.getByte(a);
            switch (b) {
                case 0x01: System.out.println("BE");  break;
                case 0x08: System.out.println("LE");  break;
                default:
                    assert false;
            }
        } finally {
            unsafe.freeMemory(a);
        }
    }
}
