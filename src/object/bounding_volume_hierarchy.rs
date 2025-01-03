use crate::geo;
use crate::object;

use geo::Point3;
use geo::Vec3;

use object::BoundingVolume;
use object::RayData;

pub struct BvhNode {
    pub volume: BoundingVolume,
    pub index: i32,
}

struct TreeNode {
    index: i32,
}

pub struct BoundingVolumeHierarchy {
    nodes: Vec<BvhNode>,
}

const NUM_SPLIT_PLANES: usize = 3;
const SPLIT_PLANES: [Vec3; NUM_SPLIT_PLANES] = [
    Vec3::new(1.0, 0.0, 0.0),
    Vec3::new(0.0, 1.0, 0.0),
    Vec3::new(0.0, 0.0, 1.0),
];

fn build_kd_tree(
    centroids: &[Point3],
    tree: &mut Vec<TreeNode>,
    indices: &mut [usize],
    split_index: usize,
) -> usize {
    tree.push(TreeNode { index: 0 });
    let node_index = tree.len() - 1;

    if indices.len() == 1 {
        let point_index = indices[0];
        tree[node_index].index = -(point_index as i32);
    } else {
        let split_plane = SPLIT_PLANES[split_index];

        let split_point = indices.len() / 2;
        indices.select_nth_unstable_by(split_point, |a, b| {
            (Vec3::from(centroids[*a]) * split_plane)
                .total_cmp(&(Vec3::from(centroids[*b]) * split_plane))
        });

        build_kd_tree(
            centroids,
            tree,
            &mut indices[..split_point],
            (split_index + 1) % NUM_SPLIT_PLANES,
        );
        tree[node_index].index = build_kd_tree(
            centroids,
            tree,
            &mut indices[split_point..],
            (split_index + 1) % NUM_SPLIT_PLANES,
        ) as i32;
    }

    return node_index;
}

fn compute_bounds<F>(
    nodes: &mut Vec<BvhNode>,
    tree: &[TreeNode],
    func: &F,
    tree_index: usize,
) -> usize
where
    F: Fn(usize) -> BoundingVolume,
{
    let tree_node = &tree[tree_index];
    nodes.push(BvhNode {
        volume: BoundingVolume::new(),
        index: 0,
    });
    let node_index = nodes.len() - 1;

    if tree_node.index <= 0 {
        let index = -tree_node.index as usize;
        nodes[node_index].index = -(index as i32);
        nodes[node_index].volume = func(index);
    } else {
        compute_bounds(nodes, tree, func, tree_index + 1);
        let volume0 = nodes[node_index + 1].volume;
        nodes[node_index].volume.include_volume(volume0);

        let new_index = compute_bounds(nodes, tree, func, tree_node.index as usize);
        nodes[node_index].index = new_index as i32;

        let volume1 = nodes[new_index].volume;
        nodes[node_index].volume.include_volume(volume1);
    }

    return node_index;
}

impl BoundingVolumeHierarchy {
    pub fn with_volumes<F>(points: &[Point3], func: &F) -> BoundingVolumeHierarchy
    where
        F: Fn(usize) -> BoundingVolume,
    {
        let mut indices = Vec::with_capacity(points.len());
        for i in 0..points.len() {
            indices.push(i);
        }

        let mut tree = Vec::with_capacity(points.len() * 2);
        build_kd_tree(&points, &mut tree, &mut indices[..], 0);

        let mut nodes = Vec::with_capacity(points.len() * 2);
        compute_bounds(&mut nodes, &tree, func, 0);

        return BoundingVolumeHierarchy { nodes };
    }

    pub fn with_nodes(nodes: Vec<BvhNode>) -> BoundingVolumeHierarchy {
        return BoundingVolumeHierarchy { nodes };
    }

    pub fn intersect<F>(&self, raydata: RayData, max_distance: f32, closest: bool, func: &mut F)
    where
        F: FnMut(usize, f32) -> Option<f32>,
    {
        #[derive(Copy, Clone)]
        struct StackEntry {
            node_index: usize,
            min_distance: f32,
        }

        let mut stack = [StackEntry {
            node_index: 0,
            min_distance: 0.0,
        }; 64];

        let mut distance = max_distance;
        let mut n = 0;
        stack[n] = StackEntry {
            node_index: 0,
            min_distance: 0.0,
        };
        n += 1;

        while n > 0 {
            n -= 1;
            let node_index = stack[n].node_index;
            let node = &self.nodes[node_index];
            let node_min = stack[n].min_distance;

            if node_min > distance {
                continue;
            }

            if node.index <= 0 {
                let index = (-node.index) as usize;
                if let Some(new_distance) = func(index, distance) {
                    distance = new_distance;
                    if !closest {
                        break;
                    }
                }
            } else {
                let indices = [node_index + 1, node.index as usize];
                let mut min_distances = [0.0; 2];
                let mut max_distances = [0.0; 2];
                for i in 0..2 {
                    if let Some((min, max)) = self.nodes[indices[i]].volume.intersect_ray(raydata) {
                        min_distances[i] = min;
                        max_distances[i] = max;
                    } else {
                        min_distances[i] = f32::MAX;
                        max_distances[i] = -f32::MAX;
                    }
                }

                for i in 0..2 {
                    let j = if min_distances[0] >= min_distances[1] {
                        i
                    } else {
                        1 - i
                    };
                    if max_distances[j] >= 0.0 {
                        stack[n].node_index = indices[j];
                        stack[n].min_distance = min_distances[j];
                        n += 1;
                    }
                }
            }
        }
    }
}
