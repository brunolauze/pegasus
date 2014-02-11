	//-- CIMInstanceProvider methods
    /** Given a reference to an instance of the CIM class, fills in the data
     *  elements of the class with the details gleaned from the system. */
    void getInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
        InstanceResponseHandler& handler);

    /** Returns filled instances for all instances of the CIM class detected
     *  on the system. */
    void enumerateInstances(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
        InstanceResponseHandler& handler);

    /** Produces a list of references to all instances of the CIM class
     *  detected on the system, but does not fill the instances
     *  themselves. */
    void enumerateInstanceNames(
        const OperationContext& context,
        const CIMObjectPath& ref,
        ObjectPathResponseHandler& handler);

    /** Currently unimplemented in the Pegasus source, this is a no-op
     *  here. */
    void modifyInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const CIMInstance& instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList& propertyList,
        ResponseHandler& handler);

    /** Currently unimplemented in the Pegasus source, this is a no-op
     *  here. */
    void createInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const CIMInstance& instanceObject,
        ObjectPathResponseHandler& handler);

    /** Currently unimplemented in the Pegasus source, this is a no-op
     *  here. */
    void deleteInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        ResponseHandler& handler);

    void initialize(CIMOMHandle& handle);
    void terminate();

    void invokeMethod(
        const OperationContext& context,
        const CIMObjectPath& objectReference,
        const CIMName& methodName,
        const Array<CIMParamValue>& inParameters,
        MethodResultResponseHandler& handler);
    
    void execQuery(
       const OperationContext& context,
       const CIMObjectPath& objectPath,
       const QueryExpression& query,
       InstanceResponseHandler& handler);


    CIMInstance testConstructInstance(
        const CIMName &clnam,
        const CIMNamespaceName &nameSpace,
        const CLASS_IMPLEMENTATION &p);

	void addManagedSystemElementProperties(CIMInstance&,CLASS_IMPLEMENTATION);