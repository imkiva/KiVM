package com.imkiva.kivm;

/**
 * @author kiva
 * @date 2018/6/10
 */
public class HashTest {
    public static void main(String[] args) {
        String x = "so, nvdia, fuck you";
        int hash = x.hashCode();
        System.out.println(hash);
        int length = 1 << 10;
        int step1 = hash ^ ((hash >>> 20) ^ (hash >>> 12));
        System.out.println(step1);
        int step2 = step1 ^ (step1 >>> 7) ^ (step1 >>> 4);
        System.out.println(step2);
        int index = step2 & (length - 1);
        System.out.println(index);
    }
}
