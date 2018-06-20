import kivm

function csSayHello()
    system.out.println("hello world")
end

function csSayHelloInt(magicNumber)
    system.out.println("hello world, I got a magic number " + magicNumber + " from Java")
end

function newMethod()
    system.out.println("Yes! I've got updated!")
end

function javaMain()
    kivm.registerNative("com.imkiva.kivm.CovScriptJNITest", "csSayHello", "()V", csSayHello)
    kivm.registerNative("com.imkiva.kivm.CovScriptJNITest", "csSayHello", "(I)V", csSayHelloInt)
    kivm.registerNative("com.imkiva.kivm.CovScriptJNITest", "oldMethod", "()V", newMethod)

    var test_class=kivm.findClass("com.imkiva.kivm.CovScriptJNITest")
    test_class.main()
end

kivm.javaStart(javaMain)