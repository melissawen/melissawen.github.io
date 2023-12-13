/**
 * drm_atomic_helper_commit_tail - commit atomic update to hardware
 * @old_state: atomic state object with old state structures
 *
 * This is the default implementation for the
 * &drm_mode_config_helper_funcs.atomic_commit_tail hook, for drivers
 * that do not support runtime_pm or do not need the CRTC to be
 * enabled to perform a commit. Otherwise, see
 * drm_atomic_helper_commit_tail_rpm().
 *
 * Note that the default ordering of how the various stages are called is to
 * match the legacy modeset helper library closest.
 */
void drm_atomic_helper_commit_tail(struct drm_atomic_state *old_state)
{
	struct drm_device *dev = old_state->dev;

	drm_atomic_helper_commit_modeset_disables(dev, old_state);

	drm_atomic_helper_commit_planes(dev, old_state, 0);

	drm_atomic_helper_commit_modeset_enables(dev, old_state);

	drm_atomic_helper_fake_vblank(old_state);

	drm_atomic_helper_commit_hw_done(old_state);

	drm_atomic_helper_wait_for_vblanks(dev, old_state);

	drm_atomic_helper_cleanup_planes(dev, old_state);
}
EXPORT_SYMBOL(drm_atomic_helper_commit_tail);

---

int drm_mode_atomic_ioctl(struct drm_device *dev,
			  void *data, struct drm_file *file_priv)
{
	struct drm_mode_atomic *arg = data;
	uint32_t __user *objs_ptr = (uint32_t __user *)(unsigned long)(arg->objs_ptr);
	uint32_t __user *count_props_ptr = (uint32_t __user *)(unsigned long)(arg->count_props_ptr);
	uint32_t __user *props_ptr = (uint32_t __user *)(unsigned long)(arg->props_ptr);
	uint64_t __user *prop_values_ptr = (uint64_t __user *)(unsigned long)(arg->prop_values_ptr);
	unsigned int copied_objs, copied_props;
	struct drm_atomic_state *state;
	struct drm_modeset_acquire_ctx ctx;
	struct drm_out_fence_state *fence_state;
	int ret = 0;
	unsigned int i, j, num_fences;

	/* disallow for drivers not supporting atomic: */
	if (!drm_core_check_feature(dev, DRIVER_ATOMIC))
		return -EOPNOTSUPP;

	/* disallow for userspace that has not enabled atomic cap (even
	 * though this may be a bit overkill, since legacy userspace
	 * wouldn't know how to call this ioctl)
	 */
	if (!file_priv->atomic) {
		drm_dbg_atomic(dev,
			       "commit failed: atomic cap not enabled\n");
		return -EINVAL;
	}

	if (arg->flags & ~DRM_MODE_ATOMIC_FLAGS) {
		drm_dbg_atomic(dev, "commit failed: invalid flag\n");
		return -EINVAL;
	}

	if (arg->reserved) {
		drm_dbg_atomic(dev, "commit failed: reserved field set\n");
		return -EINVAL;
	}

	if (arg->flags & DRM_MODE_PAGE_FLIP_ASYNC) {
		drm_dbg_atomic(dev,
			       "commit failed: invalid flag DRM_MODE_PAGE_FLIP_ASYNC\n");
		return -EINVAL;
	}

	/* can't test and expect an event at the same time. */
	if ((arg->flags & DRM_MODE_ATOMIC_TEST_ONLY) &&
			(arg->flags & DRM_MODE_PAGE_FLIP_EVENT)) {
		drm_dbg_atomic(dev,
			       "commit failed: page-flip event requested with test-only commit\n");
		return -EINVAL;
	}

	state = drm_atomic_state_alloc(dev);
	if (!state)
		return -ENOMEM;

	drm_modeset_acquire_init(&ctx, DRM_MODESET_ACQUIRE_INTERRUPTIBLE);
	state->acquire_ctx = &ctx;
	state->allow_modeset = !!(arg->flags & DRM_MODE_ATOMIC_ALLOW_MODESET);

retry:
	copied_objs = 0;
	copied_props = 0;
	fence_state = NULL;
	num_fences = 0;

	for (i = 0; i < arg->count_objs; i++) {
		uint32_t obj_id, count_props;
		struct drm_mode_object *obj;

		if (get_user(obj_id, objs_ptr + copied_objs)) {
			ret = -EFAULT;
			goto out;
		}

		obj = drm_mode_object_find(dev, file_priv, obj_id, DRM_MODE_OBJECT_ANY);
		if (!obj) {
			drm_dbg_atomic(dev, "cannot find object ID %d", obj_id);
			ret = -ENOENT;
			goto out;
		}

		if (!obj->properties) {
			drm_dbg_atomic(dev, "[OBJECT:%d] has no properties", obj_id);
			drm_mode_object_put(obj);
			ret = -ENOENT;
			goto out;
		}

		if (get_user(count_props, count_props_ptr + copied_objs)) {
			drm_mode_object_put(obj);
			ret = -EFAULT;
			goto out;
		}

		copied_objs++;

		for (j = 0; j < count_props; j++) {
			uint32_t prop_id;
			uint64_t prop_value;
			struct drm_property *prop;

			if (get_user(prop_id, props_ptr + copied_props)) {
				drm_mode_object_put(obj);
				ret = -EFAULT;
				goto out;
			}

			prop = drm_mode_obj_find_prop_id(obj, prop_id);
			if (!prop) {
				drm_dbg_atomic(dev,
					       "[OBJECT:%d] cannot find property ID %d",
					       obj_id, prop_id);
				drm_mode_object_put(obj);
				ret = -ENOENT;
				goto out;
			}

			if (copy_from_user(&prop_value,
					   prop_values_ptr + copied_props,
					   sizeof(prop_value))) {
				drm_mode_object_put(obj);
				ret = -EFAULT;
				goto out;
			}

			ret = drm_atomic_set_property(state, file_priv,
						      obj, prop, prop_value);
			if (ret) {
				drm_mode_object_put(obj);
				goto out;
			}

			copied_props++;
		}

		drm_mode_object_put(obj);
	}

	ret = prepare_signaling(dev, state, arg, file_priv, &fence_state,
				&num_fences);
	if (ret)
		goto out;

	if (arg->flags & DRM_MODE_ATOMIC_TEST_ONLY) {
		ret = drm_atomic_check_only(state);
	} else if (arg->flags & DRM_MODE_ATOMIC_NONBLOCK) {
		ret = drm_atomic_nonblocking_commit(state);
	} else {
		ret = drm_atomic_commit(state);
	}

out:
	complete_signaling(dev, state, fence_state, num_fences, !ret);

	if (ret == -EDEADLK) {
		drm_atomic_state_clear(state);
		ret = drm_modeset_backoff(&ctx);
		if (!ret)
			goto retry;
	}

	drm_atomic_state_put(state);

	drm_modeset_drop_locks(&ctx);
	drm_modeset_acquire_fini(&ctx);

	return ret;
}
