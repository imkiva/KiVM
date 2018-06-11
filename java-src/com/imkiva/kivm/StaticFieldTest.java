package com.imkiva.kivm;

class Person {
	static int i;
}

interface PersonContainer {
	Person p = new Person();
}

class StaticFieldTest extends Person implements PersonContainer {

	public void print() {
		i = 4;
		super.i = 2;
		System.out.println(Person.i);
	}

	public static void main(String[] args) {
		StaticFieldTest u = new StaticFieldTest();
		u.i = 3;
		u.print();
		Person.i = 10;
		System.out.println(u.i);
		System.out.println(Person.i);
	}
}

