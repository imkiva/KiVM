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

kivm.javaInit()

kivm.registerNative("com.imkiva.kivm.CovScriptJNITest", "csSayHello", "()V", csSayHello)
kivm.registerNative("com.imkiva.kivm.CovScriptJNITest", "csSayHello", "(I)V", csSayHelloInt)
kivm.registerNative("com.imkiva.kivm.CovScriptJNITest", "oldMethod", "()V", newMethod)

kivm.javaMain("com.imkiva.kivm.CovScriptJNITest", {})

kivm.javaDestroy()


