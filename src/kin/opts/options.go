package opts

// supported types of coordinators.
const (
	SingleClient = 0
	Mpi
	Backend
)

const (
	Memory = 0
	Rados
	Redis
)

/*
	when more than one client is modifying the content of a version, this option specifies the level of
	synchronization that the coordinator is providing with respect to having each client know about which
	objects are being handled by whom (i.e. at what point in time do clients exchange metadata about the
	objects being modified by each).

	@c NONE doesn't share any information. This is useful for applications that "know what they're doing",
	which implies that user is responsible for managing object-in-version containment metadata. In other
	words, the refdb only stores the parent-child relationship of versions but not of what each version
	contains.

	@c AT_CREATE synchronizes information right after a version has been created, that is, after a
	::Repository::create

	@c AT_COMMIT synchronizes information right after a version has been committed, that is, after the user
	executes ::Repository::commit.

	@c AT_ADD_OR_REMOVE synchronizes every time an application adds or removes an object from a revision, i.e.
	after every @c Repository::add() or @c Repository::remove().

	Default: @c ClienSync::AT_EACH_COMMIT
*/
const (
	None = 0
	AtCreate
	AtCommit
	AtAddOrRemove
)

// comma-separated list of URIs. @see http://cpp-netlib.org/0.10.1/in_depth/uri.html
type uri string

type Options struct {
	// name of repository
	RepositoryName string

	// type of coordinator
	CoordinatorType int

	// type of object database to use. Default: Memory
	ObjectDbType int

	// type of object database to use. Default: Memory
	MetaDbType int

	// address of the metadb server, for remote-based implementations
	MetaDbServerAddress uri

	// address of the objdb server, for remote-based implementations
	ObjDbServerAddress uri

	// whether to initialize the metadb if empty
	MetaDbInitializeIfEmpty bool

	// what to use to seed the internal hashing algorithm
	HashSeed string

	ClientSyncMode int

	// For @c MPI coordinator class, this denotes the leader rank options for mpi coordination. Default: 0
	//mpi_leader_rank

	// MPI communicator used by the @c MPI coordinator class. Assumes that the underlying type representing
	// communicators is an int. Default: NULL.
	//void* mpi_comm

	// Rados pool name for the object db
	RadosObjDbPoolName string

	// Rados pool name for the metadata db. Default: "data".
	RadosMetaDbPoolName string
}

func NewOptions() (o Options) {
	o.CoordinatorType = SingleClient
	o.ObjectDbType = Memory
	o.MetaDbType = Memory
	o.ClientSyncMode = AtCommit
	o.RadosMetaDbPoolName = "data"
	o.RadosObjDbPoolName = "data"
	return
}
