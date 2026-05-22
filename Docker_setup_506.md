# ECE 506: Program 1 — Building and Running GPGPU-Sim using Docker with CUDA

## 1. Install Docker

- Visit the official Docker website: [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop)
- Download and install Docker Desktop appropriate for your operating system. **Windows users can use WSL to get a uniform experience when running applications on the server.**

> **Note:** After installation, confirm Docker is working by running `docker --version` in a terminal window.

## 2. Pull the Pre-built GPGPU-Sim Docker Image

Open a terminal and run:

```bash
docker pull mrjeffery/gpgpusim-course:cuda_10_1
```

This command downloads a pre-built Docker image containing:

- GPGPU-Sim source code
- CUDA Toolkit 10.1
- Necessary dependencies and build tools

## 3. Launch a Docker Container

You have two options for launching the container: a quick one-liner, or a `docker-compose.yml` file that also lets you share files between your host machine and the container.

### Option A: Quick Launch

```bash
docker run -w /root -it mrjeffery/gpgpusim-course:cuda_10_1 /bin/bash
```

This gets you into the container immediately, but any files you create inside will be lost when the container stops.

### Option B: Using Docker Compose (Recommended)

Docker Compose lets you mount a folder from your host machine into the container. This means you can edit files on your host (using your favorite editor or IDE) and they will appear inside the container automatically — and vice versa. Your work is saved on your host even after the container stops.

**Step 1:** Create a file named `docker-compose.yml` in your project directory with the following content:

```yaml
services:
  gpgpusim:
    image: mrjeffery/gpgpusim-course:cuda_10_1
    container_name: gpgpusim506
    volumes:
      - /path/to/your/dir:/root
    command: tail -f /dev/null
```

**Step 2:** Replace `/path/to/your/dir` with the actual path to a folder on your machine where you want to keep your work. For example: `./my-work:/root` maps your local `my-work` folder to `/root` inside the container.

**Step 3:** Start the container in the background:

```bash
docker compose up -d
```

**Step 4:** Open a shell inside the running container:

```bash
docker exec -it gpgpusim506 /bin/bash
```

You are now inside the container. Any files you place in `/root` will also appear in your host folder, and any files you place in your host folder will appear in `/root`.

**Step 5:** When you are done, simply run `exit` inside the container. When you exit the container, it will keep
running in the background, so you can enter it again whenever you want. You should not be
creating new containers. To find the container that you worked on, run `docker container
ls -a` and see a list of all your containers and their container IDs. To enter a container, run
docker exec -it ContainerID /bin/bash

## 4. Check Out and Prepare GPGPU-Sim Source Code

Once inside the container, navigate to the GPGPU-Sim source directory:

```bash
cd ~/gpgpu-sim_distribution/
git checkout tags/v4.0.1 -b v401
```

Run `ls` to list the files. You should see the root directory of the GPGPU-Sim source code.

## 5. Compile GPGPU-Sim

Run the following commands to clean, configure, and compile the simulator:

```bash
make clean
source setup_environment
make -j4
```
