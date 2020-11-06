# mdns
**General todos:**
- [ ] Refactor to allow multiple queries to be run
- [ ] Let wpacks be passed as a directory 
- [ ] Fix directories for the file transfer


## dockerized-build

To build local containers, use dockerfiles provided at
pkg/${component}/Dockerfile. For example the mdns-controller can be
built from the project root with `docker build -f
pkg/mdns-controller/Dockerfile ./`

## docker-compose test network

A compose file (docker-compose.yml) is provided in the project root to
generate a reproducible test environment. The compose environment can
be started with `docker-compose up`, and containers are available at
the provider and requester names for testing with `docker exec -it
provider bash` and `docker exec -it requester bash`.

To rebuild the compose containers after making local changes, you need
to manually run `docker-compose down` and `docker-compose
build`. Commands can be chained when developing, with `docker-compose
down && docker-compose build && docker-compose up` corresponding to a
complete environment recreation and rebuild.
