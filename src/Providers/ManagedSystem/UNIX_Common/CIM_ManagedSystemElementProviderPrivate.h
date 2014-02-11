	// private member to store handle passed by initialize()
    CIMOMHandle _cimomHandle;

    Array<CIMKeyBinding> _constructKeyBindings(const CLASS_IMPLEMENTATION&);

    // Used to add properties to an instance
    // first argument is the class of instance to be built
    // second argument is a Process instance that contains
    // process status information that has been fetched
    CIMInstance _constructInstance(
        const CIMName &clnam,
        const CIMNamespaceName &nameSpace,
        const CLASS_IMPLEMENTATION &p);

    // checks the class passed by the cimom and throws
    // an exception if it's not supported by this provider
    void _checkClass(CIMName&);


    void __initialize(CIMOMHandle &ch);

    String _getBaseClassName() const;
    String _getClassName() const;
    CIMName _getBaseClassCimName() const;
    CIMName _getClassCimName() const;

    // uninitialized; will be set during initialize() processing
    String _hostName;

    // uninitialized; will be set during initialize() processing
    String _osName;
